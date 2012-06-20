import os
import pickle
import subprocess as sp
blacklist = {"gl_init.c":True}
endings = [".c",".cpp"]
includes = "-I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads"
link = "-L/opt/vc/lib/  -lEGL -lGLESv2 -lbcm_host -lvcos"
output = "start"
try:
    with open("pycache","r") as fptr:
        pycache = pickle.load(fptr)
except:
    pycache = {}

try:
	os.mkdir("./obj")
except:
	pass


def get_files_to_compile():
    objs = os.listdir("./src/")
    compfiles = []
    compobjs = []
    for obj in objs:
    	if obj in blacklist:
    		continue
        if obj[0] == ".":
            continue
        for end in endings:
            
            lend = len(end)
            endsyms = obj[-lend:]
            if endsyms == end:
                compfiles.append(obj)
                compobjs.append("./obj/" + obj[:-lend] + ".o")
    return compfiles,compobjs
ftc,ftl = get_files_to_compile()




for i in ftc:
    if i in blacklist:
    	print "Blacklist caught",i
    	continue
    chtime = os.path.getmtime("./src/"+i)
    dot_point = i.rfind('.')
    i_stripped = i[:dot_point]
    print ">",i_stripped
    print chtime
    if i not in pycache.keys() or pycache[i] < chtime and not i in blacklist:
        
        if False and i[-2:] == ".c":
            cc = "gcc"
        else:
            cc = "g++"
        call = "{2} -c ./src/{0} {1} -O3 -o ./obj/{3}.o".format(i,includes,cc,i_stripped)
        print call
        #os.system(call)
        try:
        	p = sp.check_call(call.split(" "))
        	pycache[i] = os.path.getmtime("./src/"+i)
        except:
        	print "Error"
        
    else:
        print "skipping",i

with open("pycache","w") as fptr:
    pickle.dump(pycache,fptr)

acc_link = "g++ {0} -o {1} -O3 ".format(link,output)
for i in ftl:
    acc_link +=i + " "
print acc_link
os.system(acc_link)