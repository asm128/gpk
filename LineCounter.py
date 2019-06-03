import os, time
from time import gmtime
import shutil


# just get the current path as root dir
rootdir=os.getcwd() 

#rootdir=input( "enter directory to search (format: drive:\\path\\) :\n" )

if rootdir.rfind("\\") != len(rootdir)-1:
	rootdir+="\\"

global nodekeys

filenode = { 'name' : '', 'folder' : '', 'blank lines' : 0, 'comment lines' : 0, 'code lines' : 0, 'directive lines' : 0,'bytes' : 0, 'creation date': 0, 'modification date': 0 }
nodekeys = [ 'name', 'folder', 'creation date', 'modification date', 'code lines', 'directive lines', 'comment lines', 'blank lines', 'bytes' ]

allnodes = list()

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


def checkFileExt( fname, ext ):
	if( fname.find(ext) == len(fname)-len(ext) ):
		return True
	else:
		return False


def getFileLines( fullpath, filename ):
	f = open(fullpath, 'r')
	try:
		lines=f.readlines()
	except:
		f.close()
		return []
	f.close()
	return lines

# loop through subdirs
for subdir, dirs, files in os.walk(rootdir):
	if( (not( subdir.lower().find( 'Win32.Release' ) >= 0 )) 
		and (not( subdir.lower().find( 'Win32.Debug' ) >= 0 )) 
		and (not( subdir.lower().find( 'x64.Release' ) >= 0 )) 
		and (not( subdir.lower().find( 'x64.Debug' ) >= 0 )) 
		and (not( subdir.lower().find( 'dxut' ) >= 0 )) 
		#and (not( subdir.lower().find( 'gunz' ) >= 0 ))
		and (not( subdir.lower().find( 'sha' ) >= 0 )) 
		and (not( subdir.lower().find( 'crypto' ) >= 0 )) 
		and (not( subdir.lower().find( 'gzip' ) >= 0 )) 
		#and (not( subdir.lower().find( 'rest' ) >= 0 ))
		and (not( subdir.lower().find( 'graphingcalculator' ) >= 0 )) 
		and (not( subdir.lower().find( 'boost' ) >= 0 )) 
		and (not( subdir.lower().find( 'obj' ) >= 0 )) 
		and (not( subdir.lower().find( 'zlib' ) >= 0 )) 
		and (not( subdir.lower().find( 'basiclock' ) >= 0 )) 
		and (not( subdir.lower().find( 'snippets' ) >= 0 )) 
		and (not( subdir.lower().find( 'external' ) >= 0 )) 
		and (not( subdir.lower().find( 'unittest' ) >= 0 )) 
		):
		
		for filename in files:
			# only count lines of code files
			newNode = { 'name' : '', 'folder' : '', 'blank lines' : 0, 'comment lines' : 0, 'code lines' : 0, 'directive lines' : 0,'bytes' : 0,
		 				'creation date':0, 'modification date': 0 }
			if( checkFileExt(filename, ".h") or checkFileExt(filename, ".cpp") or checkFileExt(filename, ".def") or checkFileExt(filename, ".c") or checkFileExt(filename, ".hpp") or checkFileExt(filename, ".fx") ):
				print(filename) # let me know the current file name for if it crashes
				fullpath 			= subdir+"\\"+filename
				newNode 			= filenode.copy()
				newNode['name'] 	= filename
				newNode['folder'] 	= subdir
				newNode['creation date']		= time.asctime( gmtime( os.path.getctime(fullpath) ) )
				newNode['modification date']	= time.asctime( gmtime( os.path.getmtime(fullpath) ) )
				lines = getFileLines( fullpath, filename )
	
				for line in lines:
					newNode=parseLine( newNode, line.replace("\0", "").strip() )
				
				allnodes.append( newNode )

outString = '';

totalcomments       = 0
totalcode          	= 0
totaldirectives     = 0
totalblanks         = 0
totallines          = 0
totalbytes			= 0

#allnodes.sort()

for node in allnodes:
	totalcomments 		+= node['comment lines']
	totalcode 			+= node['code lines']
	totaldirectives 	+= node['directive lines']
	totalblanks 		+= node['blank lines']
	totallines			+= node['comment lines'] + node['blank lines'] + node['directive lines'] + node['code lines']
	totalbytes			+= node['bytes'] 
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

outString += "Average lines per file: " + str(totallines/len(allnodes)) + "\n"
outString += "Average lines of comments per file: " + str(totalcomments/len(allnodes)) + "\n"
outString += "Average lines of code per file: " + str(totalcode/len(allnodes)) + "\n"
outString += "Average lines of compiler directives per file: " + str(totaldirectives/len(allnodes)) + "\n"
outString += "Average blank lines per file: " + str(totalblanks/len(allnodes)) + "\n\n"

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
