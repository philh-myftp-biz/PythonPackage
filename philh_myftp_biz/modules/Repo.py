from git.exc import InvalidGitRepositoryError as InvalidRepoError
from ..pc import Path

class Repo:

    def __init__(self, path:Path) -> None:
        from git.exc import NoSuchPathError
        from git import Repo

        self.path = Path(path)
        
        try:
            self._repo = Repo(str(path))
        except NoSuchPathError:
            raise FileNotFoundError(self.path) from None

        self.diff = self._repo.index.diff

        self.commit = self._repo.index.commit

        self.head = self._repo.head

        self.init = self._repo.init

        self.new_tag = self._repo.create_tag

        self.REMOTE = self._repo.remotes[0]

        self.push = self.REMOTE.push

        self.reset = self._repo.git.reset
        """Unstage all files"""

    def refresh(self):
        self.rm('.', recurse=True, force=True, cached=True)
        self.add('.')

    def rm(self, 
        path: str,
        *,
        cached: bool = False,
        recurse: bool = True,
        force: bool = False
    ):
        flags = []
        if cached:  flags += ['--cached']
        if recurse: flags += ['-r']
        if force:   flags += ['-f']

        self._repo.git.rm(*flags, path)

    def focus(self, path:str):

        # Reset the index to clear any manually staged files
        self.reset()

        # Stage only the specific subfolder
        self.add(path)

    def add(self, path:str):
        """Stage specific subfolder"""
        self._repo.git.add(path)

    @property
    def changes(self) -> int:
        return len(self.diff(self.head.commit))
    
    def update_submodules(self, *,
        remote: bool = True,
        force: bool = False,
    ) -> None:

        args = ['update', '--recursive', '--init']

        if remote: args += ['--remote']
        if force: args += ['--force']
        
        self._repo.git.submodule(*args)

