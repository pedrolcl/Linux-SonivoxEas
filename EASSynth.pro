#------------------------
#
# Sonivox EAS Synthesizer
#
#------------------------

TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += sonivox \
           libsvoxeas \
           cmdlnsynth \
           guisynth

