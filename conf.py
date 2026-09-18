# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

from os.path import abspath
from pathlib import Path
import sys

sys.setrecursionlimit(5000) 

sys.path.insert(0, abspath("."))

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'philh_myftp_biz'
copyright = '2026, Phil Hunt'
author = 'Phil Hunt'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    "autodoc2",
    "sphinx.ext.viewcode",
]

autodoc2_packages = ["philh_myftp_biz"]
autodoc2_output_dir = "_api"
autodoc2_render_plugin = "rst"

autodoc_mock_imports = [
    ".".join(pyi_path.with_suffix("").parts)
    for pyi_path in
    Path("philh_myftp_biz").rglob("*.pyi") 
]

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'alabaster'

