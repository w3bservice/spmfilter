list(APPEND COMMON_LIBS)

add_executable(spm_unit_tests
  suite.c
  test_core.c
  test_message.c
  test_modules.c
  test_nexthop.c
  test_ea.c
  test_envelope.c
  test_dict.c
  test_session.c
)
target_link_libraries(spm_unit_tests smf ${COMMON_LIBS} ${CHECK_LIB})
ADD_TEST(SMFUnitTests spm_unit_tests)

add_executable(test_settings test_settings.c)
target_link_libraries(test_settings smf ${COMMON_LIBS})
ADD_TEST(SMFSettings_T ${EXECUTABLE_OUTPUT_PATH}/test_settings)

add_executable(test_list test_list.c)
target_link_libraries(test_list smf ${COMMON_LIBS})
ADD_TEST(SMFList_T ${EXECUTABLE_OUTPUT_PATH}/test_list)

add_executable(test_header test_header.c)
target_link_libraries(test_header smf ${COMMON_LIBS})
ADD_TEST(SMFHeader_T ${EXECUTABLE_OUTPUT_PATH}/test_header)

add_executable(test_part test_part.c)
target_link_libraries(test_part smf ${COMMON_LIBS})
ADD_TEST(SMFPart_T ${EXECUTABLE_OUTPUT_PATH}/test_part)

add_executable(test_smtp test_smtp.c)
target_link_libraries(test_smtp smf ${COMMON_LIBS})
ADD_TEST(smf_smtp ${EXECUTABLE_OUTPUT_PATH}/test_smtp)

add_executable(test_internal test_internal.c)
target_link_libraries(test_internal smf ${COMMON_LIBS})
ADD_TEST(smf_internal ${EXECUTABLE_OUTPUT_PATH}/test_internal)

add_executable(test_pipe test_pipe.c)
target_link_libraries(test_pipe smf pipe ${COMMON_LIBS})
ADD_TEST(smf_pipe ${EXECUTABLE_OUTPUT_PATH}/test_pipe)

add_executable(test_smtpd test_smtpd.c ../src/smf_server.c)
target_link_libraries(test_smtpd smf smtpd ${COMMON_LIBS})
ADD_TEST(smf_smtpd ${EXECUTABLE_OUTPUT_PATH}/test_smtpd)

if(HAVE_DB4)
	add_executable(test_lookup_db4 test_lookup_db4.c)
	target_link_libraries(test_lookup_db4 smf ${COMMON_LIBS} db)
	ADD_TEST(smf_lookup_db4 ${EXECUTABLE_OUTPUT_PATH}/test_lookup_db4)
endif(HAVE_DB4)

if(HAVE_LDAP)
	add_executable(test_lookup_ldap test_lookup_ldap.c)
        target_link_libraries(test_lookup_ldap smf ${COMMON_LIBS} ldap)
        ADD_TEST(smf_lookup_ldap ${EXECUTABLE_OUTPUT_PATH}/test_lookup_ldap)
endif(HAVE_LDAP)

if(HAVE_ZDB)
	add_executable(test_lookup_sql test_lookup_sql.c)
        target_link_libraries(test_lookup_sql smf ${COMMON_LIBS} zdb)
        ADD_TEST(smf_lookup_sql ${EXECUTABLE_OUTPUT_PATH}/test_lookup_sql)
	add_executable(test_lookup_sqlite test_lookup_sqlite.c)
        target_link_libraries(test_lookup_sqlite smf ${COMMON_LIBS} zdb)
        ADD_TEST(smf_lookup_sqlite ${EXECUTABLE_OUTPUT_PATH}/test_lookup_sqlite)
endif(HAVE_ZDB)

add_library(testmod1 SHARED testmod1.c)
target_link_libraries(testmod1 smf)

add_library(testmod2 SHARED testmod2.c)
target_link_libraries(testmod2 smf)