#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

COMPONENT_SRCDIRS := .
CFLAGS += -DSET_TRUSTED_CERT_IN_SAMPLES
