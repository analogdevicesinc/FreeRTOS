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
import win32com.client as win32

__usage__ = """ \

Usage:
  Formatter.py  

Description:
	-z --zip         Zip the contents of the generated Footprint directory into the given zip name. 
	-v --versionfile File containing the svn version number.

Warnings:

Assumptions:
  
Options:
  -h,   --help      Display usage information (this message)
"""


#*********************************************************************************
# Function: GetTestDataToHTML(xmlfile, processor)
# Description:  Creates html text from an xml document.  Test data is presented in 
#               a table, with the processor name as a heading.
#*********************************************************************************
def GetTestDataToHTML(xmlfile, proc):
	#Proc header above the table
	text = ''
	
	#Table next
	text += '<table border="1">' + '\n'
	text += '<tr>'               + '\n'
	text += '<th>Test</th>'      + '\n'
	text += '<th>Data</th>'      + '\n'
	text += '<th>Code</th>'      + '\n'
	text += '<th>Total</th>'     + '\n'
	text += '</tr>'              + '\n'

	
	dom = parse(xmlfile)
	els = dom.getElementsByTagName(proc) 
	for el in els:
		tests = el.getElementsByTagName('Test')
		for test in tests:
			text += '</tr>'             + '\n'
			text += '<td>' + test.getAttribute('Text')   + '</td>\n'
			text += '<td>' + test.getAttribute('Data')   + '</td>\n'
			text += '<td>' + test.getAttribute('Code')   + '</td>\n'
			text += '<td>' + test.getAttribute('Total')  + '</td>\n'
			text += '</tr>'             + '\n'
	
	text += '</table>'           + '\n'
	return text

###################################################################################
# Function: GetTestDataToXls(xmlfile, processor)
# Description:  Creates xls text from an xml document.  Test data is presented in
#               a table, with the processor name as a heading.
# 
###################################################################################
def GetTestDataToXls(directory, xmlfile, processor, nwc, rom):
	
	proc = processor
	line = 1	
	xl = win32.gencache.EnsureDispatch('Excel.Application')
	xl.DefaultSaveFormat = win32.constants.xlWorkbookNormal
	
	# Create a new workbook with a single sheet.
	ss = xl.Workbooks.Add(1)
	sh = ss.ActiveSheet
	# Call the sheet after the test type
	sh.Name = "Footprint"
	
	xl.Visible = True
	
	#Let's start with the header
	if (rom == 2):
		proc = processor.replace('-ROM', '')
		sh.Cells(line,1).Value = proc
		sh.Cells(line,1).Font.Bold = 1
		sh.Cells(line,1).Font.Size = 22
		line += 1
		sh.Cells(line,1).Font.Size = 14
		sh.Cells(line,1).Value = "ROM Configuration2"
	elif (nwc == 1):
		proc = processor.replace('-NWC', '')
		sh.Cells(line,1).Value = processor
		sh.Cells(line,1).Font.Bold = 1
		sh.Cells(line,1).Font.Size = 22
	elif (g_griffarm == 1):
		proc = processor.replace('-ARM', '')
		sh.Cells(line,1).Value = proc + ' (ARM)'
		sh.Cells(line,1).Font.Bold = 1
		sh.Cells(line,1).Font.Size = 22
	elif (g_griffsharc == 1):
		proc = processor.replace('-SHARC', '')
		sh.Cells(line,1).Value = processor + ' (SHARC+)'
		sh.Cells(line,1).Font.Bold = 1
		sh.Cells(line,1).Font.Size = 22
	else:
		proc = processor
		sh.Cells(line,1).Value = proc
		sh.Cells(line,1).Font.Bold = 1
		sh.Cells(line,1).Font.Size = 22
	line += 1
	
	#Now a description of the results
	sh.Cells(line,1).Font.Size = 14
	sh.Cells(line,1).Value = "uC/OS-III Sizing Metrics"
	line += 1
	sh.Cells(line,1).Font.Size = 11
	sh.Cells(line,1).Value = "All values in bytes."
	line += 3
	
	#Now start listing the tests and column headers, e.g.
	#                        
	# Test             Data  Code   Total
	sh.Cells(line,1).Value = "Test"
	sh.Cells(line,1).Font.Size = 11
	sh.Cells(line,1).Font.Bold = 1
	sh.Cells(line,2).Value = "Data"
	sh.Cells(line,2).Font.Size = 11
	sh.Cells(line,2).Font.Bold = 1
	sh.Cells(line,3).Value = "Code"
	sh.Cells(line,3).Font.Size = 11
	sh.Cells(line,3).Font.Bold = 1
	sh.Cells(line,4).Value = "Total"
	sh.Cells(line,4).Font.Size = 11
	sh.Cells(line,4).Font.Bold = 1
	
	line = OutputDataToXls(xmlfile, proc, sh, line, rom)
	# Now autofit the page.
	sh.Columns("A:A").EntireColumn.AutoFit()
	
	#Now add the notes at the end.
	line += 1
	sh.Cells(line,1).Value = 'Notes'
	sh.Cells(line,1).Font.Bold = 1
	sh.Cells(line,1).Font.Size = 11
	line += 1
	sh.Cells(line,1).Value = '"Basic Project" contains the minimum code required to create a single task and run the scheduler.'
	line += 1
	sh.Cells(line,1).Value = '"Basic Project" for each object type (e.g. Flags), also has single API calls to create, post and pend on that object.'
	line += 1
	if g_griffarm == 0:
		sh.Cells(line,1).Value = 'Projects with "ipa" enabled use the -ipa flag during compilation, instructing the compiler to perform interprocedural analysis.'
		line += 1
	sh.Cells(line,1).Value = 'Test applications use statically-allocated stacks, each sized to 400 CPU words (4 bytes).'
	line += 1
	sh.Cells(line,1).Value = ' - Single (User) Task'
	line += 1
	sh.Cells(line,1).Value = ' - Idle Task'
	line += 1
	sh.Cells(line,1).Value = ' - Tick Task'
	line += 1
	if rom > 0:
		sh.Cells(line,1).Value = ' - Timer Task'
	else:
		sh.Cells(line,1).Value = ' - Statistics Task (when enabled)'
	line += 1
	sh.Cells(line,1).Value = 'Application built in "Release" mode'
	line += 1
	sh.Cells(line,1).Value = 'Compiler optimisation set to "optimise for space".'
	line += 1
	if rom > 0:
		sh.Cells(line,1).Value = 'Test applications built with the -utility-rom flag set.'
		line += 1
		sh.Cells(line,1).Value = 'The data required for all available ROM features is included in every project.'
		line += 1
		sh.Cells(line,1).Value = 'The data required for all available ROM features is included in every project.'
		line += 1
	
	filename = os.path.join(directory, processor + '.xls')
	print 'Saving xls results to ' + filename
	#ss.SaveAs(filename, 56)
	ss.SaveCopyAs(filename)
	ss.Close(False)
	xl.Application.Quit()
	
	# I don't trust the current save to happen immediately - scripting multiple writes
	# seems to cause a bit of a problem.  Let's wait a little bit.
	time.sleep(3)


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
def OutputDataToXls(xmlfile, proc, sheet, startrow, rom):
	irow = startrow
	if (rom != 2): 
		objects = ['NONE', 'MSG', 'FLG', 'MUT', 'SEM', 'ALL', 'STAT']
		names =   ['Test', 'Message Queues', 'Flags', 'Mutexes', 'Semaphores', 'Semaphore + Mutex + Message Queue + Flag', 'Statistics']
	else:
		# No statistics task for BF707 ROM2
		objects = ['NONE', 'MSG', 'FLG', 'MUT', 'SEM', 'ALL']
		names =   ['Test', 'Message Queues', 'Flags', 'Mutexes', 'Semaphores', 'Semaphore + Mutex + Message Queue + Flag']
	
	index = 0
	
	dom = parse(xmlfile)
	els = dom.getElementsByTagName(proc) 
	for el in els:
		for object in objects:
			sheet.Cells(irow,1).Value = names[index]
			sheet.Cells(irow,1).Font.Bold = 1
			index += 1
			irow  += 1
			tests = el.getElementsByTagName('Test')
			for test in tests:
				if  (test.getAttribute('Type') == object) and (test.getAttribute('ipa') == '0') and not (test.getAttribute('NumObjects') == '2'):
					# Output the Basic case with only one object assigned.
					sheet.Cells(irow,1).Value = test.getAttribute('Text')
					sheet.Cells(irow,2).Value = test.getAttribute('Data')
					sheet.Cells(irow,3).Value = test.getAttribute('Code')
					sheet.Cells(irow,4).Value = test.getAttribute('Total')
				elif  (test.getAttribute('Type') == object) and (test.getAttribute('ipa') == '0') and (test.getAttribute('NumObjects') == '2'):
					# Output the Basic case with two objects assigned.
					sheet.Cells(irow +1 ,1).Value = test.getAttribute('Text')
					sheet.Cells(irow +1 ,2).Value = test.getAttribute('Data')
					sheet.Cells(irow +1 ,3).Value = test.getAttribute('Code')
					sheet.Cells(irow +1 ,4).Value = test.getAttribute('Total')
				elif  (test.getAttribute('Type') == object) and (test.getAttribute('ipa') == '1'):
						# Output the Basic case with one objects assigned and using IPA.
						# Annoyingly, the basic case doesn't have a 'two object' option, 
						# so ipa has a different relative position.
						if(object == 'NONE') or (object == 'STAT'):
							sheet.Cells(irow +1 ,1).Value = test.getAttribute('Text')
							sheet.Cells(irow +1 ,2).Value = test.getAttribute('Data')
							sheet.Cells(irow +1 ,3).Value = test.getAttribute('Code')
							sheet.Cells(irow +1 ,4).Value = test.getAttribute('Total')
						else:
						# Output the Basic case with one objects assigned and using IPA.
							sheet.Cells(irow +2 ,1).Value = test.getAttribute('Text')
							sheet.Cells(irow +2 ,2).Value = test.getAttribute('Data')
							sheet.Cells(irow +2 ,3).Value = test.getAttribute('Code')
							sheet.Cells(irow +2 ,4).Value = test.getAttribute('Total')
			# Add a blank line seperator
			if object == 'NONE':
				irow += 2
			else:
				irow += 3
	
	return irow



#*********************************************************************************
# Function: GetTestDataToCSV(xmlfile, processor)
# Description:  Creates csv-format text from an xml document.  Test data is 
#               presented with the processor name as a heading.
#*********************************************************************************
def GetTestDataToCSV(xmlfile, proc):
	#Proc header above the results
	text = proc + '\n'
	text += 'uCOS Sizing Metrics' + '\n'
	text += 'All values in bytes.' + '\n'
	text += '\n'
	
	objects = ['NONE', 'MSG', 'FLG', 'MUT', 'SEM', 'ALL', 'STAT']
	names =   ['Test', 'Message Queues', 'Flags', 'Mutexes', 'Semaphores', 'Semaphore + Mutex + Message Queue + Flag', 'Statistics']
	
	index = 0
	
	dom = parse(xmlfile)
	els = dom.getElementsByTagName(proc) 
	for el in els:
		for object in objects:
			text += names[index] + '\n'
			index += 1
			tests = el.getElementsByTagName('Test')
			row1 = ''
			row2 = ''
			row3 = ''
			for test in tests:
				if  (test.getAttribute('Type') == object) and (test.getAttribute('ipa') == '0') and not (test.getAttribute('NumObjects') == '2'):
					# Output the Basic case with only one object assigned.
					row1 = test.getAttribute('Text') + ',' 
					row1 += test.getAttribute('Data') + ','
					row1 += test.getAttribute('Code') + ','
					row1 += test.getAttribute('Total')
				elif  (test.getAttribute('Type') == object) and (test.getAttribute('ipa') == '0') and (test.getAttribute('NumObjects') == '2'):
					# Output the Basic case with two objects assigned.
					row2 = test.getAttribute('Text') + ',' 
					row2 += test.getAttribute('Data') + ','
					row2 += test.getAttribute('Code') + ','
					row2 += test.getAttribute('Total')
				elif  (test.getAttribute('Type') == object) and (test.getAttribute('ipa') == '1'):
						# Output the Basic case with one objects assigned and using IPA.
						# Annoyingly, the basic case doesn't have a 'two object' option, 
						# so ipa has a different relative position.
						if(object == 'NONE') or (object == 'STAT'):
							row2 = test.getAttribute('Text') + ',' 
							row2 += test.getAttribute('Data') + ','
							row2 += test.getAttribute('Code') + ','
							row2 += test.getAttribute('Total')
						else:
						# Output the Basic case with one objects assigned and using IPA.
							row3 = test.getAttribute('Text') + ',' 
							row3 += test.getAttribute('Data') + ','
							row3 += test.getAttribute('Code') + ','
							row3 += test.getAttribute('Total')
			text += row1 + '\n'
			text += row2 + '\n'
			if row3 != '':
				text += row3 + '\n'
			# Add a blank line seperator
			text += '\n'
	
	print text
	#Now add the notes at the end.
	text += 'Notes' + '\n'
	text += '"Basic Project" contains the minimum code required to create a single task and run the scheduler.' + '\n'
	text += 'Projects with "ipa" enabled use the -ipa flag during compilation instructing the compiler to perform interprocedural analysis.' + '\n'
	text += 'Test applications use statically-allocated stacks each sized to 400 CPU words (4 bytes).' + '\n'
	text += ' - Single (User) Task' + '\n'
	text += ' - Idle Task' + '\n'
	text += ' - Tick Task' + '\n'
	text += ' - Statistics Task (when enabled)' + '\n'
	text += 'Application built in "Release" mode' + '\n'
	text += 'Compiler optimisation set to "optimise for space".' + '\n'
	
	return text



	
def GetIntro():
	text = ''
	
	f = open('intro.txt' , 'r')
	
	line = f.readline()
	while not line == '':
		text += line
		line = f.readline()
	f.close()
	
	return text

def GetConditions():
	text = ''
	
	f = open('conditions.txt' , 'r')
	
	line = f.readline()
	while not line == '':
		text += line
		line = f.readline()
	f.close()
	
	return text

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
	global g_griffarm 
	global g_griffsharc 
	
	basedir =  os.getcwd()
	g_proc = 'ADSP-21469'	
	use_rom = 0
	
	format = 'xls'
	zipt = 0 
	g_griffarm = 0 
	g_griffsharc = 0 
	zipfile = '' 
	verfile = '' 
	vervalue = '' 
	
	try:
		opts, args = getopt.getopt(argv, 'hrascf:p:z:v:', ['help', 'rom', 'arm', 'sharcplus', 'csv', 'file=', 'proc=', 'zip=', 'version='])
	except getopt.GetoptError:
		usage()
		return
	for opt, arg in opts:
		if opt in ('-h', '--help'):
			usage()
			return
		if opt in ('-r', '--use_rom'):
			use_rom = 1
			print 'Using ROM'
		if opt in ('-a', '--arm'):
			g_griffarm = 1
			print 'Using arm'
		if opt in ('-s', '--sharc'):
			g_griffsharc = 1
			print 'Using Sharc+'
		if opt in ('-c'):
			format = 'csv'
			print 'Output to csv'
		if opt in ('-f', '--file'):
			g_inputfile = os.path.normpath(arg)
			print 'Input file= ' + g_inputfile
		if opt in ('-p', '--proc'):
			g_proc = arg
			print 'Processor = ' + g_proc
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
		
	
	xlsdirectory = os.path.join(basedir, 'Footprint')
	
	if not os.path.exists(xlsdirectory):
		os.mkdir(xlsdirectory)
	
	if zipt == 1:
		#zip the Footprint folder
		zipfile += '_svn' + vervalue
		print 'Zipping : ' + zipfile 
		print str(shutil.make_archive(zipfile, 'zip', xlsdirectory))
		return
	
	g_outputfile = os.path.join(basedir, g_proc, 'results.xml') 
	if not os.path.exists(g_outputfile):
		print ' No results file for ' + g_proc 
		print ' Cannot format.  Quitting. '
		return
	
	
	rom = 0
	nwc = 0
	
	# See if this processor is a Normal Word project, and set the appropriate flag.
	if not g_proc.find('-NWC')  == -1:
		nwc = 1
	
	# See if this processor is a ROM project, and set the appropriate flag.
	if not g_proc.find('-ROM')  == -1:
		rom = 2
	
	text = ''
	resultsfile = ''
	header = ''
	conditions = ''
	
	if format == 'xls':
		text = GetTestDataToXls(xlsdirectory, g_outputfile, g_proc, nwc, rom)
		return
	elif format == 'html':
		header = '<h2>' + g_proc + '</h2>' + '\n'
		header += GetIntro()
		text = GetTestDataToHTML(g_outputfile, g_proc)
		conditions = GetConditions()
		resultsfile = g_proc + '.html'
	else:
		print 'Generating CVS'
		text = GetTestDataToCSV(g_outputfile, g_proc)
		resultsfile = g_proc + '.csv'
	
	f = open(resultsfile , 'w')
	f.write(text)
	f.close()

	# Now pretty-up the xml file
	dom = parse(g_outputfile)
	f = open(g_outputfile, 'w')
	f.write(dom.toprettyxml())
	f.close()
	
	
#*********************************************************************************
if __name__ == '__main__':
	main()
	exit(0)
