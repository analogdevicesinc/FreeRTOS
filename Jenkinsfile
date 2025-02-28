#!/usr/bin/env groovy
@Library('jenkins_shared_lib') _

pipeline {
	agent { label 'dte-docker' }
    stages {
        stage('run-scan (freertos)') {
            steps {
                script { 
                    synopsys.detect { 
                        configFile = 'blackduck.config' 
                        failBuild = true
                    } 
                }
            }
        }
        stage('run-scan (osal)') {
            steps {
                script { 
                    synopsys.detect { 
                        configFile = 'blackduck.osal.config' 
                        failBuild = true
                    } 
                }
            }
        }
        stage('generate-bom (freertos)') {
            steps {
                script {
                    bdcli.generate_bom {
                        output         = "bd-freertos/dte-freertos.json"
                        projectName    = "DTE-FreeRTOS"
                        projectVersion = "2.2.0"
                        recursive      = true
                    }
                    bdcli.cof_review {
                        bom  = "bd-freertos/dte-freertos.json"
                        html = "bd-freertos/dte-freertos_cof_review.html"
                    }
                    bdcli.cof_appendix {
                        bom  = "bd-freertos/dte-freertos.json"
                        docx = "bd-freertos/dte-freertos.docx"
                        html = "bd-freertos/dte-freertos_appendix.html"
                    }
                    bdcli.legal_review {
                        bom  = "bd-freertos/dte-freertos.json"
                        html = "bd-freertos/dte-freertos_legal_review.html"
                    }
                }
            }
        }	
        stage('generate-bom (osal)') {
            steps {
                script {
                    bdcli.generate_bom {
                        output         = "bd-osal/dte-freertos-osal.json"
                        projectName    = "DTE-FreeRTOS-OSAL-DFP"
                        projectVersion = "1.0.0"
                        recursive      = true
                    }
                    bdcli.cof_review {
                        bom  = "bd-osal/dte-freertos-osal.json"
                        html = "bd-osal/dte-freertos-osal_cof_review.html"
                    }
                    bdcli.cof_appendix {
                        bom  = "bd-osal/dte-freertos-osal.json"
                        docx = "bd-osal/dte-freertos-osal.docx"
                        html = "bd-osal/dte-freertos-osal_appendix.html"
                    }
                    bdcli.legal_review {
                        bom  = "bd-osal/dte-freertos-osal.json"
                        html = "bd-osal/dte-freertos-osal_legal_review.html"
                    }
                }
            }
        }	
        stage('generate packs') {
            steps {
                script {
                    sh "scripts/generate_pack_files.sh"
                    archiveArtifacts artifacts: 'packs/*.pack', fingerprint: true  
                }
            }
        }
        stage('package') {
            steps {
                script {
                    sh "scripts/packageFreeRTOS.sh"
                    zip zipFile: 'freertos/freertos_2.2.0_full.zip',    archive: false, dir: 'output/freertos_2.2.0_full'
                    zip zipFile: 'freertos/freertos_2.2.0_a5.zip',      archive: false, dir: 'output/git/freertos_2.2.0_cortexa5'
                    zip zipFile: 'freertos/freertos_2.2.0_a55.zip',     archive: false, dir: 'output/git/freertos_2.2.0_cortexa55'
                    zip zipFile: 'freertos/freertos_2.2.0_m33.zip',     archive: false, dir: 'output/git/freertos_2.2.0_m33'
                    zip zipFile: 'freertos/freertos_2.2.0_m33_IAR.zip', archive: false, dir: 'output/git/freertos_2.2.0_m33_IAR'
                    zip zipFile: 'freertos/freertos_2.2.0_sharc.zip',   archive: false, dir: 'output/git/freertos_2.2.0_sharc'
                    zip zipFile: 'freertos/freertos_2.2.0_blackfin.zip',   archive: false, dir: 'output/git/freertos_2.2.0_blackfin'
                    zip zipFile: 'freertos/freertos_2.2.0_sharcfx.zip', archive: false, dir: 'output/git/freertos_2.2.0_sharcfx'

                    zip zipFile: 'examples/freertos_examples_2.2.0_m33_ccces.zip', archive: false, dir: 'examples/ADSP-SC83x/M33/CCES'
                    zip zipFile: 'examples/freertos_examples_2.2.0_m33_iar.zip', archive: false, dir: 'examples/ADSP-SC83x/M33/IAR'
                    zip zipFile: 'examples/freertos_examples_2.2.0_sharcfx.zip', archive: false, dir: 'examples/ADSP-SC83x/SHARC-FX'

                    archiveArtifacts artifacts: 'freertos/*.zip', fingerprint: true
                    archiveArtifacts artifacts: 'examples/*.zip', fingerprint: true
                }
            }
        }
    }
}
