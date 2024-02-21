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
        }
      } // steps
    } // Setup

  stages {
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
        }
      } // steps
    } // Setup

    stage('Build and Docs') {
      parallel {
        stage('Build') {
          steps {
            withEnv(["XMOS_CMAKE_PATH=${WORKSPACE}/xcommon_cmake"]) {
              withVenv {
                withTools(params.TOOLS_VERSION) {
                  dir("host_xvf_control") {
                    sh "cmake -B build -G Ninja && cmake --build build"
                  }
                  dir("sw_audio_dsp/host_cmd_map") {
                    sh "cmake -B build -G Ninja && cmake --build build"
                  }
                  sh 'cp host_xvf_control/build/xvf_host host_xvf_control/build/*.so sw_audio_dsp/host_cmd_map/build'
                  zip dir: "sw_audio_dsp/host_cmd_map/build", zipFile: "host_control_linux.zip", archive: true
                  dir("sw_audio_dsp") {
                    dir("app_dsp_xk_evk_xu316") {
                      // check the notebook is clean
                      sh "jupyter nbconvert --clear-output dsp_design.ipynb --output=clean && diff dsp_design.ipynb clean.ipynb"
                      // run the notebook and save the output
                      sh "jupyter nbconvert --execute --ExecutePreprocessor.allow_errors=True --to html --output=xk_evk_xu316 dsp_design.ipynb"
                      archiveArtifacts artifacts: "xk_evk_xu316.html"
                      sh 'find bin -name "*.xe" | grep .'  // fails if binaries don't exist
                      zip dir: "bin", zipFile: "xk_evk_xu316_bin.zip", archive: true
                    }
                    dir("app_dsp_xk_316_mc") {
                      // check the notebook is clean
                      sh "jupyter nbconvert --clear-output dsp_design.ipynb --output=clean && diff dsp_design.ipynb clean.ipynb"
                      // run the notebook and save the output
                      sh "jupyter nbconvert --execute --ExecutePreprocessor.allow_errors=True --to html --output=xk_316_mc dsp_design.ipynb"
                      archiveArtifacts artifacts: "xk_316_mc.html"
                      sh 'find bin -name "*.xe" | grep .'  // fails if binaries don't exist
                      zip dir: "bin", zipFile: "xk_316_mc_bin.zip", archive: true
                    }
                  }
                }
              }
            }
          }
        } // Build

        stage('Docs') {
          steps {
            sh """docker run -u "\$(id -u):\$(id -g)" \
                  --rm \
                  -v ${WORKSPACE}/sw_audio_dsp:/build \
                  ghcr.io/xmos/xmosdoc:$XMOSDOC_VERSION -v"""
            dir('sw_audio_dsp') {
              archiveArtifacts artifacts: "doc/_out/pdf/*.pdf"
              archiveArtifacts artifacts: "doc/_out/html/**/*"
              sh 'find doc/_out/pdf -type f -not -name "*.pdf" -exec rm {} +'  // delete latex junk
              zip zipFile: "sw_audio_dsp_docs.zip", archive: true, dir: "doc/_out", exclude: "linkcheck/**"
            }
          } // steps
        } // Docs
      } // parallel
    } // build and docs

    stage('Archive sandbox') {
      steps {
        // remove all junk and archive
        sh 'for dir in $(find -type d -name ".git"); do git -C $dir/.. clean -xdf; done'
        // delete symlinks
        sh 'find -type l -delete'
        // delete non-git dirs
        sh 'for dir in $(ls -A); do if [ ! -d $dir/.git ]; then rm -rf $dir; fi; done'
        zip zipFile: "sw_audio_dsp_sandbox.zip", archive: true
      }
    } // Archive sandbox
  } // stages
  post {
    cleanup {
      xcoreCleanSandbox()
    }
  }
}
