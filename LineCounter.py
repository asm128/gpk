CHECKABLE_EXTS = [".h", ".cpp", ".def", ".c", ".hpp", ".fx", ".ts", ".js", ".html"]
SKIPPABLE_DIRS = [".pio", ".git", "EspSoftwareSerial-8.1.0", "AsyncWebServer"]

import os, time
from time import gmtime
import shutil

def fixPythonPath(stupid_python_path):
	return stupid_python_path.replace("\\", "/")

# just get the current path as root dir
rootdir = fixPythonPath(os.getcwd())

#rootdir=input( "enter directory to search (format: drive:/path/) :\n" )

if rootdir.rfind("/") != len(rootdir)-1:
	rootdir += "/"

global nodekeys

filenode = { 'name' : '', 'folder' : '', 'blank lines' : 0, 'comment lines' : 0, 'code lines' : 0, 'directive lines' : 0,'bytes' : 0, 'creation date': 0, 'modification date': 0 }
nodekeys = [ 'name', 'folder', 'creation date', 'modification date', 'code lines', 'directive lines', 'comment lines', 'blank lines', 'bytes' ]

allnodes = list()

def checkFileDir(fdir, skippable):
	return -1 != fdir.find("/" + skippable);

def checkFileExt(fname, ext):
	return fname[0] != '.' and (fname.find(ext) == len(fname) - len(ext))

def getFileLines( fullpath):
	f = open(fullpath, 'r')
	try:
		lines=f.readlines()
	except:
		f.close()
		return []
	f.close()
	return lines

def parseLine(fnode, line):
	if line.find("//") <= 3 and line.find("//") != -1 and len(line) > 4 and line.find("++") == -1:
		fnode['comment lines']+=1
	elif( len(line) > 4 or line.find("++") != -1) and line.find("#") != 0:
		fnode['code lines']+=1
	elif line.find("#") == 0:
		fnode['directive lines']+=1
	else:
		fnode['blank lines']+=1
	fnode['bytes'] += len(line)
	return fnode

# loop through subdirs
for subdir, dirs, files in os.walk(rootdir):
	subdir = fixPythonPath(subdir)
	if [checkFileDir(subdir, dir) for dir in SKIPPABLE_DIRS].count(True):
		continue;
	
	for filename in [fixPythonPath(fn) for fn in files]:
		if 0 == [checkFileExt(filename, ext) for ext in CHECKABLE_EXTS].count(True):
			continue;

		# only count lines of code files
		newNode = { 'name' : '', 'folder' : '', 'blank lines' : 0, 'comment lines' : 0, 'code lines' : 0, 'directive lines' : 0,'bytes' : 0, 'creation date':0, 'modification date': 0 }
		
		print(filename) # let me know the current file name for if it crashes
		fullpath			= subdir + "/" + filename
		newNode				= filenode.copy()
		newNode['name']		= filename
		newNode['folder']	= subdir
		newNode['creation date']		= time.asctime( gmtime( os.path.getctime(fullpath) ) )
		newNode['modification date']	= time.asctime( gmtime( os.path.getmtime(fullpath) ) )
		lines = getFileLines(fullpath)

		for line in lines:
			newNode=parseLine( newNode, line.replace("\0", "").strip() )

		allnodes.append( newNode )

outString		= "";

totalcomments	= 0
totalcode		= 0
totaldirectives	= 0
totalblanks		= 0
totallines		= 0
totalbytes		= 0

maxcomments		= 0
maxcode			= 0
maxdirectives	= 0
maxblanks		= 0
maxlines		= 0
maxbytes		= 0

#allnodes.sort()

for node in allnodes:
	totalcomments		+= node['comment lines']
	totalcode			+= node['code lines']
	totaldirectives		+= node['directive lines']
	totalblanks			+= node['blank lines']
	totallines			+= node['comment lines'] + node['blank lines'] + node['directive lines'] + node['code lines']
	totalbytes			+= node['bytes']
 
	maxcomments			= max(maxcomments	, node['comment lines'])
	maxcode				= max(maxcode		, node['code lines'])
	maxdirectives		= max(maxdirectives	, node['directive lines'])
	maxblanks			= max(maxblanks		, node['blank lines'])
	maxlines			= max(maxlines		, node['comment lines'] + node['blank lines'] + node['directive lines'] + node['code lines'])
	maxbytes			= max(maxbytes		, node['bytes'])

	for key in nodekeys:
		outString += str( 'File '+ str(key) + ': '+ str(node[key]) + '\n')
	outString += '\n'

outString += "\n----------------------------- Total Files: " + str(len(allnodes))
outString += "\n----------------------------- Total Lines: " + str(totallines) + '\n\n'
outString += "Comments: " + str(totalcomments) + '\n'
outString += "Code: " + str(totalcode) + '\n'
outString += "Compiler directives: " + str(totaldirectives) + '\n'
outString += "Blanks: " + str(totalblanks) + '\n'
outString += "Total bytes: " + str(totalbytes) + '\n'

outString += '\n----------------------------- Stats: \n\n'

outString += "\nAverage lines per file: " + str(totallines/len(allnodes))
outString += "\nAverage lines of comments per file: " + str(totalcomments/len(allnodes))
outString += "\nAverage lines of code per file: " + str(totalcode/len(allnodes))
outString += "\nAverage lines of compiler directives per file: " + str(totaldirectives/len(allnodes))
outString += "\nAverage blank lines per file: " + str(totalblanks/len(allnodes))
outString += "\n"

outString += "\nMax lines in a file: "							+ str(maxlines)
outString += "\nMax lines of comments in a file: "				+ str(maxcomments)
outString += "\nMax lines of code in a file: "					+ str(maxcode)
outString += "\nMax lines of compiler directives in a file: "	+ str(maxdirectives)
outString += "\nMax blank lines in a file: "					+ str(maxblanks)
outString += "\n"

outString += "Comments: " + str(100.0/totallines*totalcomments) + "%\n"
outString += "Code: " + str(100.0/totallines*totalcode) + "%\n"
outString += "Compiler directives: " + str(100.0/totallines*totaldirectives) + '%\n'
outString += "Blanks: " + str(100.0/totallines*totalblanks) + '%\n'

print(outString)

mytime = time.gmtime()


#f1=open(rootdir+ "Lines_"+ time.asctime( time.time() ).replace(':', '_').replace(" ", "_") + ".txt", "w")
f1=open(rootdir+ "Lines_" + str(mytime.tm_year) + "_" + str(mytime.tm_mon) + "_" + str(mytime.tm_mday) + "_" + str(mytime.tm_hour) + "_" + str(mytime.tm_min) + ".txt", "w")
f1.write( outString )
f1.close()
