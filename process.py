import os
import sys
from pathlib import Path

Path(sys.argv[1]).mkdir(parents=True, exist_ok=True)


inread = open(sys.argv[1]+".CSV", "r")
out = open(sys.argv[1] + "/" + sys.argv[1]+".gpx", "w")

out.write('<?xml version="1.0" encoding="UTF-8"?>\n')
out.write('<gpx version="1.1" creator="Garmin Connect"><trk>\n')
out.write('<name>Let c. ' + str() + '</name>\n')
out.write("<trkseg>\n")



first = True

for line in inread:
    if first is True:
        first = False
        continue
    
    
    record = line.split(";")
     
    out.write('<trkpt lon="'+ str(float(record[1])/1000) + '" lat="'+ str(float(record[0])/1000) + '">\n')
    out.write(' <time>2022-01-01T' + record[5][:-1] + 'Z</time> \n')
    out.write(' <ele>' + record[2] +  '</ele> \n')
    out.write(' <speed>' + record[3] +  '</speed> \n')   
    
    out.write('</trkpt>\n')
     
     
     
     




out.write("</trkseg>\n")
out.write("</trk>\n")
out.write("</gpx>\n")

inread.close()
out.close()

runCMD = "Rscript --vanilla graph.r " + sys.argv[1]
os.system(runCMD)


