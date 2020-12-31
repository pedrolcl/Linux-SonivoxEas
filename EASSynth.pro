#------------------------
#
# Sonivox EAS Synthesizer
#
#------------------------

TEMPLATE = subdirs
SUBDIRS += sonivox \
           libsvoxeas \
           cmdlnsynth \
           guisynth

libsvoxeas.depends = sonivox
cmdlnsynth.depends = libsvoxeas
guisynth.depends = libsvoxeas
