pipelineJob('HSS_CCDM_EDA_authprovvalidator') {
    concurrentBuild(false)
    logRotator(-1, 15, 1, -1)

    parameters {
        stringParam ('GERRIT_BRANCH', 'master', 'Use this parameter to select your repository BRANCH.')
        stringParam ('GERRIT_REFSPEC', 'master', 'Use same value as GERRIT_BRANCH')
    }

    triggers {
        gerrit {
            events {
                patchsetCreated()
                changeMerged()
            }

            project('plain:HSS/CCDM/EDA/authprovvalidator', ["ANT:**"])

            configure { gerritTrigger ->
                new groovy.util.Node(gerritTrigger, 'serverName', 'adp')
            }

            buildSuccessful(null, null)
        }
    }

    definition {
        cpsScm {
            scm {
                git {
                    remote {
                        url ('https://madridci@${COMMON_GERRIT_URL}/a/HSS/CCSM/ci')
                        credentials ('userpwd-hss')
                        branch('master')
                        refspec('')
                    }
                    extensions {
                        wipeOutWorkspace()
                    }
                }
                lightweight (false)
                scriptPath ("Jenkinsfile.cpp")
            }
        }
    }
}
