@Library('xmos_jenkins_shared_library@v0.28.0') _

def runningOn(machine) {
  println "Stage running on:"
  println machine
}

getApproval()

pipeline {
  agent {
    label 'linux&&x86_64'
  }

  options {
    skipDefaultCheckout()
    timestamps()
    buildDiscarder(xmosDiscardBuildSettings(onlyArtifacts=true))
  }

  parameters {
    string(
      name: 'TOOLS_VERSION',
      defaultValue: '15.2.1',
      description: 'The XTC tools version'
    )
  } // parameters

  environment {
    XMOSDOC_VERSION = "v5.1.1"
  } // environment

  stages {
    stage('Setup') {
      steps {
        runningOn(env.NODE_NAME)
        dir("fwk_io") {
          checkout scm
          sh 'git submodule update --init --recursive'
        }
        createVenv("fwk_io/test/requirements.txt")
        dir("fwk_io") {
          // build everything
          withVenv {
            withTools(params.TOOLS_VERSION) {
              sh "pip install -Ur test/requirements.txt"
            }
          }
        }
      } //steps
    } // stage

    stage('Build HIL tests') {
      steps {
        dir("fwk_io/test") {
          // build everything
          withVenv {
            withTools(params.TOOLS_VERSION) {
              sh "build_hil_tests.sh"
            }
          }
        }
      } // steps
      post {
        cleanup {
          xcoreCleanSandbox()
        }
      } // post
    } // stage
  } // stages
}

