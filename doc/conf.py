"""Sphinx configuration file for an LSST stack package.

This configuration only affects single-package Sphinx documenation builds.
"""

from documenteer.sphinxconfig.stackconf import build_package_configs
import lsst.pex.policy


_g = globals()
_g.update(build_package_configs(
    project_name='pex_policy',
    version=lsst.pex.policy.version.__version__))
