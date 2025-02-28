#*********************************************************************************
#  Global Definitions
#
#*********************************************************************************
from xml.dom.minidom import parse
from xml.dom.minidom import getDOMImplementation
from win32com.client import gencache
from pywintypes import com_error
from shutil import copy2
import pythoncom, time, msvcrt, os,string,threading
import sys
import getopt
import struct
import warnings
import shutil
import subprocess

__usage__ = """ \

Usage:
  CCESConfig.py 

Description:
    Create the projects for a given processor to test.   Creates a folder called
    <processor> with the test projects in place for testing.  Also copies
    and configures a config.txt file for TTH.
    For processors that use SWC in SHARC, two sets of projects are created:
        - A SWC version.
        - A NWC version (the generated folder is appended with '-NWC'.
    For BF70x processors, two sets of projects are created: 
        - One with uCOS in RAM.
        - One that uses ROM Configuration2 (the generated folder is appended 
          with '-ROM2'.
Options:
  -p,   --processor    The processor to make the projects for, e.g. ADSP-BF512.
  -f,   --family       The processor family to make the projects for.   
                       Either 'Blackfin', 'Sharc' or 'Arm'.
  -d    -- dualcore    Whether the family is dual core or not, e.g. Devonshire.
  -s    -- si          Force the silicon revision to a specific value.
  -e    -- emulator    Use the emulator debug session to run the test.  Must
                       be used for a specific processor. Options are:
                       hpusb, usb, 100b, 1000, or 2000.

EXAMPLE:
 For specific processors:
python CCESConfig.py -p ADSP-BF532 -f Blackfin 
python CCESConfig.py -p ADSP-BF609 -f Blackfin -d
python CCESConfig.py -p ADSP-21469 -f Sharc 
python CCESConfig.py -p ADSP-BF707 -f Blackfin -s 0.0 
python CCESConfig.py -p ADSP-SC589 -f Sharc  
python CCESConfig.py -p ADSP-SC589 -f Arm  

 For an entire family: 
 Singlecores:
python CCESConfig.py -f Blackfin 
python CCESConfig.py -f Sharc 
 Dualcores:
python CCESConfig.py -f Blackfin -d  

"""


g_emulator = ''

g_Projnames = ['FreeRTOS_FLGISR_Timings', 
            'FreeRTOS_ISR_Timings', 
            'FreeRTOS_MSGISR_Timings', 
            'FreeRTOS_MUT_Timings', 
            'FreeRTOS_SEMISR_Timings']

g_GriffArmNames = ['FreeRTOS_FLGISR_Timings_Core0', 
            'FreeRTOS_ISR_Timings_Core0', 
            'FreeRTOS_MSGISR_Timings_Core0', 
            'FreeRTOS_MUT_Timings_Core0', 
            'FreeRTOS_SEMISR_Timings_Core0']

g_GriffStartCore1 = 'JustRun_Core0'
 
g_GriffSharcNames = ['JustRun_Core0',
            'FreeRTOS_FLGISR_Timings_Core1', 
            'FreeRTOS_ISR_Timings_Core1', 
            'FreeRTOS_MSGISR_Timings_Core1', 
            'FreeRTOS_MUT_Timings_Core1', 
            'FreeRTOS_SEMISR_Timings_Core1']

g_Sharcs = ['ADSP-21160', 
            'ADSP-21161', 
            'ADSP-21261', 
            'ADSP-21262', 
            'ADSP-21266', 
            'ADSP-21362', 
            'ADSP-21363', 
            'ADSP-21364', 
            'ADSP-21365', 
            'ADSP-21366', 
            'ADSP-21367', 
            'ADSP-21368', 
            'ADSP-21369', 
            'ADSP-21371', 
            'ADSP-21375', 
            'ADSP-21467', 
            'ADSP-21469', 
            'ADSP-21477', 
            'ADSP-21478', 
            'ADSP-21479', 
            'ADSP-21483', 
            'ADSP-21486', 
            'ADSP-21487', 
            'ADSP-21488', 
            'ADSP-21489', 
            'ADSP-21569',
            'ADSP-SC589',
            'ADSP-SC594',
            'ADSP-SC573'] 


g_Arms = ['ADSP-SC589',
            'ADSP-SC573',
            'ADSP-SC594']

g_Blackfins =  ['ADSP-BF504', 
            'ADSP-BF504F', 
            'ADSP-BF506F', 
            'ADSP-BF512', 
            'ADSP-BF514', 
            'ADSP-BF516', 
            'ADSP-BF518', 
            'ADSP-BF522', 
            'ADSP-BF523', 
            'ADSP-BF524', 
            'ADSP-BF525', 
            'ADSP-BF526', 
            'ADSP-BF527', 
            'ADSP-BF531', 
            'ADSP-BF532', 
            'ADSP-BF533', 
            'ADSP-BF534', 
            'ADSP-BF536', 
            'ADSP-BF537', 
            'ADSP-BF538', 
            'ADSP-BF539', 
            'ADSP-BF542', 
            'ADSP-BF542M', 
            'ADSP-BF544', 
            'ADSP-BF544M', 
            'ADSP-BF547', 
            'ADSP-BF547M', 
            'ADSP-BF548', 
            'ADSP-BF548M', 
            'ADSP-BF549', 
            'ADSP-BF549M', 
            'ADSP-BF592-A',
            'ADSP-BF700', 
            'ADSP-BF701', 
            'ADSP-BF702', 
            'ADSP-BF703', 
            'ADSP-BF704', 
            'ADSP-BF705', 
            'ADSP-BF706', 
            'ADSP-BF707' ] 

g_DualCores = ['ADSP-BF561', 
            'ADSP-BF606', 
            'ADSP-BF607', 
            'ADSP-BF608', 
            'ADSP-BF609']

g_RomBlackfins = ['ADSP-BF701', 
            'ADSP-BF702', 
            'ADSP-BF703', 
            'ADSP-BF704', 
            'ADSP-BF705', 
            'ADSP-BF706'] 

g_VisaSharcs = ['ADSP-21467',
            'ADSP-21469',
            'ADSP-21477',
            'ADSP-21478',
            'ADSP-21479',
            'ADSP-21483',
            'ADSP-21486',
            'ADSP-21487',
            'ADSP-21488',
            'ADSP-21489',
            'ADSP-21569']

# Copy the config file from the source directory into the newly-created
# destination folder.
#
def CreateConfig(proc, dst, src, nwc, rom, dirName):
    print 'Creating config file for ' + proc
    template = os.path.join(src, 'config.tmplate')
    output =  os.path.join(src, 'config.txt')
    
    fin  = open(template, 'r')
    fout = open(output, 'w')
    
    logfilename = proc.lower() + '-test-output'
    
    line = ' '
    while not line == '':
        line = fin.readline()
        line = line.replace('PROCESSOR_TEMPLATE', proc)
        line = line.replace('SESSION_TEMPLATE', g_emulator)
        line = line.replace('LOGFILE_TEMPLATE', logfilename)
        line = line.replace('PROJECT_TEMPLATE', dirName)
        if nwc == 1:
            line = line.replace('PROCESSORTEST_TEMPLATE', proc + '-NWC')
        elif rom > 0:
            line = line.replace('PROCESSORTEST_TEMPLATE', proc + '-ROM' + str(rom))
        else:
            line = line.replace('PROCESSORTEST_TEMPLATE', proc)
        fout.writelines(line)
    
    fin.close()
    fout.close()

# Create a new project.
#  TODO must not hardcode path for CCES
#
def CreateProject(src, dst, proc, pname, si, nwc):

    srcprojects = src
    dstprojects = dst
    
    print '\n'
    print '###Creating project ' + pname 

    # Only set the si revision if it is given,
    if (si != ''):
        sistring = ' -revision ' + si
    else:
        sistring = ''

    # splurge out the headless builder command.    
    cmd = '"%s" '%g_CCES_HOME    
    cmd += '-nosplash -consoleLog -application com.analog.crosscore.headlesstools ' 
    cmd +=  sistring
    cmd += ' -data ' + '"' + dstprojects + '"'
    cmd += ' -project ' + '"' + srcprojects + '"'
    cmd += ' -processor ' + proc + ' -regensrc -cleanBuild all'
    
    # For Sharc we might want to use Normal Word, or Short Word (when it can be specified).
    if nwc == 1:
        cmd += ' -remove-switch compiler -swc' 
        cmd += ' -append-switch compiler -nwc' 
    
    print cmd
    logfile = os.path.join(dst, pname.lower() + '_createlog.txt')
    print 'Build output directed to ' + logfile + '\n'
    sys.stdout.flush()

    fh1 = open(logfile, 'a+')
    
    p1 = subprocess.Popen(cmd, stdout=fh1)
    returncode = p1.wait()
    fh1.close()

    attempts = 1
    while (returncode != 0) and (attempts < 6):
        print 'Build returned ' + str(returncode)
        sys.stdout.flush()
        time.sleep(2)
        print 'Deleting ' + os.path.join(dst, '.metadata')
        print 'Deleting ' + os.path.join(dst, pname)
        try:
            str(shutil.rmtree(os.path.join(dst, '.metadata')))
            str(shutil.rmtree(os.path.join(dst, pname)))
        except:
            print 'UNABLE TO DELETE FILES OR FOLDERS - MAY CAUSE PROBLEMS WITH REBUILD'
        sys.stdout.flush()
        fh1 = open(logfile, 'a+')
        
        p1 = subprocess.Popen(cmd, stdout=fh1)
        returncode = p1.wait()
        fh1.close()
        attempts += 1
    if returncode == 0:
        print 'Project successfully created.\n'
    else: 
        print 'Build returned ' + str(returncode) + '. ERRORS WITH BUILD. \n'


# Force update of the project resources.
#
def UpdateProject(proj, direct): 
    cmd2 = '"%s" '%g_CCES_HOME    
    cmd2 += '-nosplash -consoleLog -application com.analog.crosscore.headlesstools' 
    cmd2 += ' -project ' + '"' + proj + '"'
    cmd2 += ' -data ' + '"' + direct + '"'
    cmd2 += ' -build release'
    
    print '\nUpdate and build the project, this checks that the project is correct.' 
    sys.stdout.flush()
    
    fname = os.path.join(direct, 'buildlog.txt')

    fff = open(fname, 'a+')
    p1 = subprocess.Popen(cmd2, stdout=fff)
    retcode = p1.wait()
    attempts = 1    
    while (retcode != 0) and (attempts < 6):
        print 'Build output directed to ' + fname + '\n'
        sys.stdout.flush()
        time.sleep(2)
        p1 = subprocess.Popen(cmd2)
        retcode = p1.wait()
        attempts += 1
    
    if retcode < 0:
        print 'Rebuild failed on attempt: ' + str(attempts) + '\n'
    else:
        print 'Rebuild PASSED on attempt: ' + str(attempts) + '\n'
    fff.close()

# Copy the system.svc file to the destination folder.  Currently this isn't
# happening properly for project copying.
#
def CopySystemSVC(src, dst): 
    oldsvc = os.path.join(src, 'system.svc')
    
    print 'Copying: '
    print oldsvc
    print 'to'
    print dst
    shutil.copy2(oldsvc, dst)

# For a given processor:
#     - Create a folder called <processor>
#     - Copy the correct source projects into that new directory.
#     - Create a config.txt for the new processor.
def CreateProcTests(family, proc, si, dualcore, nwc, rom):
    
    if (family == 'Blackfin') and (dualcore == 1):
        srcfolder = os.path.join(os.getcwd(), 'BlackfinDualCore')
        projnames = g_Projnames
    elif (family == 'Blackfin') and (dualcore == 0) and (rom == 0):
        srcfolder = os.path.join(os.getcwd(), 'BlackfinSingleCore')
        projnames = g_Projnames
    elif (family == 'Blackfin') and (rom == 2):
        srcfolder = os.path.join(os.getcwd(), 'BigDeltaROM2')
        projnames = g_Projnames
    elif (family == 'Sharc') and (dualcore == 0):
        srcfolder = os.path.join(os.getcwd(), 'Sharc')
        projnames = g_Projnames
    elif (family == 'Arm') and (dualcore == 1):
        srcfolder = os.path.join(os.getcwd(), 'ARM')
        projnames = g_GriffArmNames
    elif (family == 'Sharc') and (dualcore == 1):
        srcfolder = os.path.join(os.getcwd(), 'SharcPlus')
        projnames = g_GriffSharcNames
    else:
        # Don't know what this is
        print 'family and dualcore options incorrect\n'
        usage()
        return
    
    if nwc == 1:
        dstfolder = os.path.join(os.getcwd(), proc + '-NWC')
        print 'NWC folder: ' + dstfolder
    elif (rom == 2):
        dstfolder = os.path.join(os.getcwd(), proc + '-ROM2')
    elif (rom == 1):
        #We don't have full support for this yet, but this is how it would look.
        dstfolder = os.path.join(os.getcwd(), proc + '-ROM1')
    elif (family == 'Arm'):
        dstfolder = os.path.join(os.getcwd(), proc + '-ARM')
    elif (family == 'Sharc') and (dualcore == 1):
        dstfolder = os.path.join(os.getcwd(), proc + '-SHARC')
    else:
        dstfolder = os.path.join(os.getcwd(), proc)
    
    # Create the folder to use
    if os.path.isdir(dstfolder):
        print 'Destination folder exists:  ' + dstfolder
        oldresultsfile = os.path.join(dstfolder, 'cycles.xml')
        if os.path.isfile(oldresultsfile):
            print 'Results file for ' + proc + ' exists, removing: ' + oldresultsfile + '\n'
            os.remove(oldresultsfile)
    else:
        print 'Creating: ' + dstfolder
        os.mkdir(dstfolder)
    
    print '\n\n\n'
    print 'Source Folder: ' + srcfolder
    print 'Destination Folder: ' + dstfolder
    sys.stdout.flush()
    
    # Special case for griffin, where we need another project that's used to start Core1
    if (family == 'Sharc') and (dualcore == 1):
        print 'Copying Initialization Project in Source Folder: ' + srcfolder
        print '...and in : ' + g_GriffStartCore1
        proj = os.path.join(srcfolder, g_GriffStartCore1)
        startproj = os.path.join(dstfolder, g_GriffStartCore1)
        shutil.copytree(proj, startproj)
    
    dirName = dstfolder.split('\\')[-1]
    for pname in projnames:
        proj = os.path.join(srcfolder, pname)
        newproject = os.path.join(dstfolder, pname)
        CreateProject(proj, dstfolder, proc, pname, si, nwc)
        CopySystemSVC(proj, newproject)
        #UpdateProject(newproject, dstfolder)
        if pname != "JustRun_Core0":
            CreateConfig(proc, newproject, proj, nwc, rom,dirName)
        else:
            continue
    
# returns a string suitable for a config.txt file, to specify an
# emulator type.  
# emulator = either 'hpusb', 'usb' or '100b'.
def CreateEmulatorString(proc, emulator):
    if emulator == '':
        return ''

    # We need to extract the end of the processor string, i.e. everything
    # after the 'ADSP-'...so we just use the characters from 5 onwards.
    processor = proc[5:].lower()

    # Now form the emulator string for the config.txt
    # it should ultimately look something like this:
    # DEBUGSESSION:<emu-bf518-hpusb>
    
    return 'DEBUGSESSION:<emu-' + processor + '-' + emulator +'>'
    
#*********************************************************************************
# usage()
#*********************************************************************************
def usage():
    sys.stdout.write(__usage__)
    
#*********************************************************************************
# main()
#*********************************************************************************
def main( argv=sys.argv[1:] ):
    global g_Blackfins
    global g_DualCores
    global g_Sharcs
    global g_Projnames
    global g_emulator
    global g_CCES_HOME
            
    cces_path = os.environ.get('CCES_HOME').strip('"').strip()
    if not cces_path:
        print "ERROR: Fail to get CCES path. Please set CCES_HOME first."
        exit(1)
    else:
        g_CCES_HOME = os.path.join(cces_path,'Eclipse\ccesc.exe')
    
    proc =  ''    
    si   =  ''    
    family = ''    
    dualcore  = 0
    
    families = ['Blackfin', 'Sharc', 'Arm']
    emulators = ['hpusb', 'usb', '100b', '1000', '2000']
    
    try:
        opts, args = getopt.getopt(argv, 'hds:p:f:e:c:', ['help', 'dualcore', 'si=', 'processor=', 'family=', 'emulator', 'clean='])
    except getopt.GetoptError:
        usage()
        return
    for opt, arg in opts:
        if opt in ('-h', '--help'):
            usage()
            return
        if opt in ('-p', '--processor'):
            proc = arg 
        if opt in ('-s', '--si'):
            si = arg 
        if opt in ('-f', '--family'):
            family = arg
        if opt in ('-d', '--dualcore'):
            dualcore = 1
        if opt in ('-e', '--emulator'):
            g_emulator = arg 
        if opt in ('-c', '--clean'):
            # Just clean up the requested folder, if there, and return.
            folder = os.path.join(os.getcwd(), arg)
            if os.path.isdir(folder):
                print 'Removing: ' + folder
                shutil.rmtree(folder, 1)
                return
            else:
                print 'No such directory: ' + folder
                return
        
    if family not in families:
        print 'Processor family "' + family + '" not recognised\n'
        usage()
        return
    
    if g_emulator not in emulators:
        usage()
        return
    
    processors = proc
    
    # You can only specify an emulator when you specify a single processor,
    # e.g. you can't say "test family blackfin, they're all connected to a hpusb".
    if (proc == '') and not (g_emulator == ''):
        print '\n\n ERROR:: If you specify an emulator, you *MUST* specify a single processor to test'
        usage()
        return
        
    g_emulator = CreateEmulatorString(proc, g_emulator)
    print 'Emulator setting: ' + g_emulator
    
    if family == 'Blackfin':
        # If a processor is specified, make sure it matches the family.
        if proc == '':
            print 'Creating projects for the entire ' + family + ' family'
            if dualcore == 1: 
                processors = g_DualCores
            else:
                processors = g_Blackfins
        elif (proc not in g_Blackfins) and (proc not in g_DualCores):
            print 'Processor ' + proc + 'not recognised as a blackfin'
            usage()
            return
    elif family == 'Sharc':
        # If a processor is specified, make sure it matches the family.
        if proc == '':
            print 'Creating projects for the entire ' + family + ' family'
            processors = g_Sharcs
        elif proc not in g_Sharcs:
            print 'Processor ' + proc + 'not recognised as a sharc'
            usage()
            return
    elif family == 'Arm':
        # If a processor is specified, make sure it matches the family.
        if proc == '':
            print 'Creating projects for the entire ' + family + ' family'
            processors = g_Arms
        elif proc not in g_Arms:
            print 'Processor ' + proc + 'not recognised as an Arm'
            usage()
    else:
        print 'Family still not recognised'
        usage()
        return
    
    print 'Processors to create: ' + str(processors)
    if proc == '':
        for processor in processors:
            print 'Creating: ' + processor
            sys.stdout.flush()
            CreateProcTests(family, processor, dualcore)
    else:
        print 'Creating: ' + proc
        sys.stdout.flush()
        CreateProcTests(family, proc, si, dualcore, 0, 0)
        if proc in g_VisaSharcs:
            print 'Creating visa projects...'
            sys.stdout.flush()
            CreateProcTests(family, proc, si, dualcore, 1, 0)
        if proc in g_RomBlackfins:
            print 'Creating ROM projects...'
            sys.stdout.flush()
            CreateProcTests(family, proc, si, dualcore, 0, 2)
    
    
#*********************************************************************************
if __name__ == '__main__':
    main()
    exit(0)

