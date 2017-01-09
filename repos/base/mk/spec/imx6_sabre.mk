#
# \brief  Build-system configurations for Wandboard Quad
# \author Stefan Kalkowski
# \date   2017-01-09
#

# denote wich specs are also fullfilled by this spec
SPECS += imx6

# add repository relative include paths
REP_INC_DIR += include/spec/imx6_sabre

# include implied specs
include $(call select_from_repositories,mk/spec/imx6.mk)
