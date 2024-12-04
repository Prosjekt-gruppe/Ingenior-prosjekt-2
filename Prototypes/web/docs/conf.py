# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

import os
import sys

# Add project files
sys.path.insert(0, os.path.abspath('../'))

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'Ingeniørprosjekt II'
copyright = '2024, Gruppe 1'
author = 'Gruppe 1'
release = 'v3.0.0'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.napoleon',
    'sphinx.ext.viewcode',
    'sphinx_js'
]

latex_elements = {
    # For estetisk tilpasning av PDF (f.eks. listings og stiler)
    'extraclassoptions': 'oneside',  # Slår av tosidig layout
    'preamble': r'''
        \usepackage{geometry}
        \geometry{a4paper, margin=1in}
        \usepackage{listings}
        \lstset{
            basicstyle=\ttfamily\small,
            breaklines=true,
            columns=fullflexible
        }
        \usepackage{tabularx}
        \let\tabulary\tabularx
        \let\endtabulary\endtabularx
    ''',
    'sphinxsetup': 'verbatimwithframe=true, verbatimhintsturnover=false'
}

# Sørg for at Sphinx bruker pdflatex med SyncTeX
latex_engine = 'pdflatex'

html_context = {
  'current_version' : "3.0",
  'versions' : [["1.0", "link to 1.0"], ["2.0", "link to 2.0"]],
}

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

js_source_path = "../app/static/js"

js_config_path = "jsdoc.json"

add_module_names = False

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']
