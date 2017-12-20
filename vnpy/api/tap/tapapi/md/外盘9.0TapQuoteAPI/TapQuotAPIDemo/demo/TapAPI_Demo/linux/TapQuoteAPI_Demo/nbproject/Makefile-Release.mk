#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/1143164949/SimpleEvent.o \
	${OBJECTDIR}/_ext/1370991913/Demo.o \
	${OBJECTDIR}/_ext/1370991913/Quote.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../../code/TapAPI/lib/linux64

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/tapquoteapi_demo

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/tapquoteapi_demo: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/tapquoteapi_demo ${OBJECTFILES} ${LDLIBSOPTIONS} -lTapQuoteAPI

${OBJECTDIR}/_ext/1143164949/SimpleEvent.o: ../../code/Demo/Common/SimpleEvent.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1143164949
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../../code/TapAPI/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1143164949/SimpleEvent.o ../../code/Demo/Common/SimpleEvent.cpp

${OBJECTDIR}/_ext/1370991913/Demo.o: ../../code/Demo/TapQuoteAPI/Demo.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1370991913
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../../code/TapAPI/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1370991913/Demo.o ../../code/Demo/TapQuoteAPI/Demo.cpp

${OBJECTDIR}/_ext/1370991913/Quote.o: ../../code/Demo/TapQuoteAPI/Quote.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1370991913
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../../code/TapAPI/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1370991913/Quote.o ../../code/Demo/TapQuoteAPI/Quote.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/tapquoteapi_demo

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
