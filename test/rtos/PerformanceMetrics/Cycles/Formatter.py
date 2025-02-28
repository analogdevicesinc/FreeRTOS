#*********************************************************************************
#  Global Definitions
#
#*********************************************************************************
from xml.dom.minidom import parse
from xml.dom.minidom import getDOMImplementation
from win32com.client import constants, gencache
from pywintypes import com_error
from shutil import copy2
import pythoncom, time, msvcrt, os,string,threading
import sys
import getopt
import struct
import warnings
import shutil
import win32com.client as win32


__usage__ = """ \

Usage:
  Formatter.py

Description:
	Takes generated cycles information and outputs to an excel file.
	The arguments should include a processor, which will indicate which
	folder to look in for results.  The results will already have been generated
	into a cycles.xml file within that folder.  The results will be taken from
	the xml file, and splurged into a excel document in a fixed format and layout.

	To handle processors that generate two sets of cycles data, then the name of
	the output folder should be used for the processor.  e.g. BF707 ROM 
	configuration is ADSP-BF707-ROM2.

	Formatter is also used to zip the results.
	
	Example usage:
		python Formatter.py -p ADSP-21469
		python Formatter.py -p ADSP-21469-NWC
		python Formatter.py -p ADSP-BF609
		python Formatter.py -p ADSP-BF518
		python Formatter.py -p ADSP-BF707-ROM2

		python Formatter.py -z Performance 
Warnings:

Assumptions:
    There is a cycles.xml file within <PerformanceMetrics/<processor-name>/*`.
Options:
  -h,   --help      Display usage information (this message)
  -p,   --proc      Processor to generate output for (actually the folder where 
                    the results live).
  -z,   --zipt      zip the contents of the following folder.					
"""


#*********************************************************************************
# Function: GetTestDataToWiki(xmlfile, processor)
# Description:  Creates wikified text from an xml document.  Test data is 
#               presented in a table, with the processor name as a heading.
#*********************************************************************************
def GetTestDataToWiki(xmlfile, proc):
	#Proc header above the table
	text = '==' + proc + '==' + '\n'
	
	#Table next
	text += '{| class="sortable"' + '\n'
	text += '! Test'              + '\n'
	text += '! Cycles'            + '\n'

	
	dom = parse(xmlfile)
	els = dom.getElementsByTagName(proc) 
	for el in els:
		tests = el.getElementsByTagName('Test')
		for test in tests:
			text += '|- '         + '\n'
			text += '| ' + test.getAttribute('Text')   + '\n'
			text += '| ' + test.getAttribute('Cycles') + '\n'
	
	text += '|}'                  + '\n'
	return text

#*********************************************************************************
# Function: GetTestDataToHTML(xmlfile, processor)
# Description:  Creates html text from an xml document.  Test data is presented in 
#               a table, with the processor name as a heading.
#*********************************************************************************
def GetTestDataToHTML(xmlfile, proc):
	#Proc header above the table
	text = '<p><big><big><b>' + proc + '</b></big></big></p>' + '\n'
	
	#Table next
	text += '<table border="1">' + '\n'
	text += '<tr>'               + '\n'
	text += '<th>Test</th>'      + '\n'
	text += '<th>Cycles</th>'    + '\n'
	text += '</tr>'              + '\n'
	
	# TPSDK has a bug that it will output some duplicate information to the console.
	# Add workaround here, to remove the same data.
	
	content = []	
	dom = parse(xmlfile)
	els = dom.getElementsByTagName(proc) 
	for el in els:
		tests = el.getElementsByTagName('Test')
		for test in tests:
			eachTestData = [test.getAttribute('Text'),test.getAttribute('Cycles')]
			if eachTestData not in content:
				content.append(eachTestData)
	for data in content:
		text += '</tr>'             + '\n'
		text += '<td>' + data[0]   + '</td>\n'
		text += '<td>' + data[1] + '</td>\n'
		text += '</tr>'             + '\n'
	
	text += '</table>'           + '\n'
	return text

###################################################################################
# Function: GetTestDataToXls(directory, xmlfile, processor, nwc, rom)
# Description:  Creates xls text from an xml document.  Test data is presented in
#               a table, with the processor name as a heading.
# 
###################################################################################
def GetTestDataToXls(directory, xmlfile, processor, nwc, rom):
	""""""
	xl = win32.gencache.EnsureDispatch('Excel.Application')
	xl.DefaultSaveFormat = win32.constants.xlWorkbookNormal
	#xl.DefaultSaveFormat = 56
	
	# Create a new workbook with a single sheet.
	ss = xl.Workbooks.Add(1)
	sh = ss.ActiveSheet
	# Call the sheet after the test type
	sh.Name = "Performance"
	
	xl.Visible = True
	
	#Let's start with the header
	if (rom == 1):
		headerstr = processor.replace('-ROM1', '')
		sh.Cells(1,1).Value = headerstr + ' ROM Configuration1'
	elif (rom == 2):
		headerstr = processor.replace('-ROM2', '')
		sh.Cells(1,1).Value = headerstr + ' ROM Configuration2'
	else:
		sh.Cells(1,1).Value = processor
	sh.Cells(1,1).Font.Bold = 1
	sh.Cells(1,1).Font.Size = 22
	
	#Now a description of the results
	sh.Cells(2,1).Font.Size = 14
	sh.Cells(2,1).Value = "uC/OS-III Performance Metrics"
	
	#Now start listing the tests and column headers, e.g.
	#                       Cycles
	# Test                  
	# test string           #number                  
	sh.Cells(4,2).Value = 'Cycles'
	sh.Cells(4,2).Font.Size = 11
	sh.Cells(4,2).Font.Bold = 1
	
	inote = OutputDataToXls(xmlfile, processor, sh, 5)
	# Now autofit the page.
	sh.Columns("A:A").EntireColumn.AutoFit()
	
	#Now add the notes at the end.
	sh.Cells(inote,1).Value = 'Notes'
	sh.Cells(inote,1).Font.Bold = 1
	sh.Cells(inote,1).Font.Size = 11
	inote += 1
	sh.Cells(inote,1).Value = 'Test applications built in "Release" mode.'
	inote += 1
	sh.Cells(inote,1).Value = 'Compiler optimisation set to "optimise for speed".'
	inote += 1
	if nwc == 1:
		sh.Cells(inote,1).Value = 'Compiler configured to build the application with -nwc.'
		inote += 1
	if rom > 0:
		sh.Cells(inote,1).Value = 'Compiler configured to build the application with -utility-rom.'
		inote += 1
		sh.Cells(inote,1).Value = 'Tests use uCOS-III ROM Configuration' + str(rom) + '.'
		inote += 1
	
	# This is a longer bit of text, we should wrap it rather than autofit the longer cell.
	sh.Cells(inote,1).Value = '"Interrupt Service Time" is the time taken from when an interrupt has latched until the start of the ISR, using adi_int for ISR installation.'
	sh.Cells(inote,1).WrapText = 1
	sh.Cells(inote,1).VerticalAlignment = win32.constants.xlTop	
	
	filename = os.path.join(directory, processor + '.xls')
	print 'Saving xls results to ' + filename
	#ss.SaveAs(filename, 56)
	ss.SaveCopyAs(filename)
	ss.Close(False)
	xl.Application.Quit()


#*********************************************************************************
# Function: OutputDataToXLS(xmlfile, processor)
# Description:  Creates xls text from the results xml document.  Test data is presented in 
#               a table, with the processor name as a heading.
#               For each object type, we output the case for:
#                   - Basic with a single object allocated.
#                   - Basic with a two objects allocated.
#                   - Basic with a one object allocated and built using -ipa
#               TODO: This could be better managed using the appropriate tags during the 
#               sizing routines.
#
#               Returns the last row to be populated.
#*********************************************************************************
def OutputDataToXls(xmlfile, proc, sheet, startrow):
	irow = startrow

	# The projects listed here is the order that they are output to the excel 
	# spreadsheet.
	projects = ['FLG', 'MSG', 'MUT', 'SEM', 'NOOS', 'ISR']
	# The headings should match the array above, but using nice words.
	heading = ['Flags', 'Message Queues', 'Mutexes', 'Semaphores', 'Interrupt Service Time', 'Interrupt Service Time']
	
	headingindex = 0
	
	dom = parse(xmlfile)
	els = dom.getElementsByTagName(proc) 
	for el in els:
		for project in projects:
			#Nothing for the ISR case - The no-OS case comes first, and it will output the header.
			if not (project == 'ISR'):
				sheet.Cells(irow,1).Value = heading[headingindex]
				sheet.Cells(irow,1).Font.Bold = 1
				irow += 1
				headingindex += 1
			tests = el.getElementsByTagName('Test')
			for test in tests:
				if  (test.getAttribute('Type') == project):
					sheet.Cells(irow,1).Value = test.getAttribute('Text')
					sheet.Cells(irow,2).Value = test.getAttribute('Cycles')
					irow += 1
			# Add a blank line seperator between groups of tests (apart from no-OS, as it is grouped with ISR)
			if not project == 'NOOS':
				irow += 1
	
	return irow


#*********************************************************************************
# Function: GetTestDataToCSV(xmlfile, processor)
# Description:  Creates csv-format text from an xml document.  Test data is 
#               presented with the processor name as a heading.
#*********************************************************************************
def GetTestDataToCSV(xmlfile, proc):
	
	projects = ['NOOS', 'ISR', 'FLG', 'MSG', 'MUT', 'SEM']
	heading = ['Interrupt Latency', 'Flags', 'Message Queues', 'Mutexes', 'Semaphores']
	
	headingindex = 0
	
	#Proc header above the results
	text = proc + '\n'
	#Now a description of the results
	text += 'uCOS Performance Metrics' + '\n'
	text += '\n'
	
	#Now start listing the tests and column headers, e.g.
	#                        RTOS
	# Test                  Cycles
	text += ',' + 'uCOS-III' + '\n'
	text += ',' + 'Cycles' + '\n'
	
	dom = parse(xmlfile)
	els = dom.getElementsByTagName(proc) 
	for el in els:
		for project in projects:
			#Print a header for the data
			if project == 'FLG':
				text +=  'Signals'
				text += '\n'
			#Nothing for the ISR case
			if not (project == 'ISR'):
				text += heading[headingindex] +','
				text += '\n'
				headingindex += 1
			tests = el.getElementsByTagName('Test')
			for test in tests:
				if  (test.getAttribute('Type') == project):
					# remove any instances of a comma - breaks the display
					desc = test.getAttribute('Text')
					desc = desc.replace(',', '')
					text += desc + ','
					text += test.getAttribute('Cycles')
					text += '\n'
			# Add a blank line seperator
			if not project == 'NOOS':
				text += '\n'
	
	# Now add the footer
	#Now add the notes at the end.
	text += '\n'
	text += 'Notes' 
	text += '\n'
	text += 'Test applications built in "Release" mode.'
	text += '\n'
	text += 'Compiler optimisation set to "optimise for speed".'
	text += '\n'
	
	return text 


#*********************************************************************************
# Function: AddTestNode(doc, parent, text, cycles):
# Description:  Creates a test child node, with 'Test' and 'Cycles' attributes.
#*********************************************************************************
def AddTestNode(doc, parent, text, cycles):
	
	node  = doc.createElement('Test')
	node.setAttribute('Text', text)
	node.setAttribute('Cycles', cycles)
	parent.appendChild(node)

#*********************************************************************************
# usage()
#*********************************************************************************
def usage():
	sys.stdout.write(__usage__)
	
#*********************************************************************************
# main()
#*********************************************************************************
def main( argv=sys.argv[1:] ):
	global g_outputfile
	global g_proc 
	
	basedir =  os.getcwd()
	g_proc = 'ADSP-21469'	
	
	format = 'xls'
	zipt = 0 
	zipfile = '' 
	nwc = 0 
	verfile = ''
	vervalue = ''
	
	# This rom setting is mainly used for the BigDelta.  It says whether the project is a rom project
	# or not.  If it is, it says which configuration is being used.  Always assume we're using RAM.
	rom = 0 
	
	try:
		opts, args = getopt.getopt(argv, 'hwcf:p:z:v:', ['help', 'file=', 'proc=', 'zip=', 'version='])
	except getopt.GetoptError:
		usage()
		return
	for opt, arg in opts:
		if opt in ('-h', '--help'):
			usage()
			return
		if opt in ('-p', '--proc'):
			g_proc = arg
		if opt in ('-w'):
			format = 'html'
		if opt in ('-c'):
			format = 'csv'
		if opt in ('-f', '--file'):
			g_inputfile = os.path.normpath(arg)
		if opt in ('-z', '--zip'):
			zipt = 1
			zipfile = os.path.normpath(arg)
		if opt in ('-v', '--version'):
			verfile = os.path.normpath(arg)
	
	# If given, make sure that the version file exists
	if (verfile != ''):
		if os.path.exists(verfile):
			# The version should be on a single line, the only thing in the file.
			vf = open(verfile, 'r')
			vervalue = vf.readline()
			vervalue = (vervalue.lstrip()).rstrip()
			print 'Version = ' + vervalue
		else:
			print 'Version file does not exist \n'
			return
	
	xlsdirectory = os.path.join(basedir, 'Performance')
	
	if not os.path.exists(xlsdirectory):
		os.mkdir(xlsdirectory)
	
	if zipt == 1:
		#zip the Footprint folder
		zipfile += '_svn' + vervalue
		print 'Zipping : ' + zipfile
		print str(shutil.make_archive(zipfile, 'zip', xlsdirectory))
		return
	
	g_outputfile = os.path.join(basedir, g_proc, 'cycles.xml') 
	
	if not os.path.exists(g_outputfile):
		print ' No results file for ' + g_proc
		print ' Cannot format.  Quitting. '
		return
	
	if not os.path.exists(xlsdirectory):
		os.mkdir(xlsdirectory)
	elif zipt == 1:
		#zip the Performance folder
		print 'Zipping : ' + zipfile
		print str(shutil.make_archive(zipfile, 'zip', xlsdirectory, xlsdirectory))
		return
	
	# See if this processor is a Normal Word project, and set the appropriate flag.
	if not g_proc.find('-NWC')  == -1:
		nwc = 1
	
	# See if this processor is a ROM project, and set the appropriate flag.
	if not g_proc.find('-ROM1')  == -1:
		rom = 1 
	# See if this processor is a ROM project, and set the appropriate flag.
	if not g_proc.find('-ROM2')  == -1:
		rom = 2 
	
	text = ''
	resultsfile = ''
	
	if format == 'xls':
		GetTestDataToXls(xlsdirectory, g_outputfile, g_proc, nwc, rom)
		return
	elif format == 'html':
		text = GetTestDataToHTML(g_outputfile, g_proc)
		resultsfile = g_proc + '-CYCLES' + '.html'
	else:
		text = GetTestDataToCSV(g_outputfile, g_proc)
		resultsfile = g_proc + '.csv'
	
	f = open(resultsfile , 'w')
	f.write(text)
	f.close()
	
	
#*********************************************************************************
if __name__ == '__main__':
	main()
	exit(0)
