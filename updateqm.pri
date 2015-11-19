# Qt/qmake doesn't compile translation files when .ts file is changed, so we
# add it ourselves, based on:
# http://www.freehackers.org/thomas/2009/03/10/fixing-qmake-missing-rule-for-ts-qm/
#
# However, this is not sufficient if we want to include the generated .qm
# files into compiled resource file because the .qrc file is initially
# compiled when running qmake. However, these rules update .qm files when
# running 'make', so the .qm files are not available when qmake is run for
# the first time. As noted below, lrelease should be also called during qmake
# using system() command:
# http://stackoverflow.com/questions/4366898/how-to-specify-auto-generated-resource-files-to-qmake
#
# However, you can run system() command after including this file so that you can
# use QMAKE_LRELEASE variable to run lrelease portably

isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}

updateqm.input = TRANSLATIONS
updateqm.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
updateqm.CONFIG += no_link target_predeps

QMAKE_EXTRA_COMPILERS += updateqm
