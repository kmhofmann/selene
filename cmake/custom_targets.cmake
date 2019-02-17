find_package(Doxygen QUIET OPTIONAL_COMPONENTS dot)

if(DOXYGEN_FOUND)
    add_custom_target(doc-generate
            COMMAND Doxygen::doxygen
            WORKING_DIRECTORY ${SELENE_DIR}
            COMMENT "Run Doxygen to generate the API documentation"
            VERBATIM
            USES_TERMINAL)
endif()

if(UNIX OR CYGWIN)
    # Requires server access.
    add_custom_target(doc-upload
            COMMAND rsync -avz --delete ./doc_gen/html/ michael@selene-lib.org:/var/www/html/selene-lib/docs
            WORKING_DIRECTORY ${SELENE_DIR}
            COMMENT "Upload doxygen-generated documentation to server."
            VERBATIM
            USES_TERMINAL)
endif()