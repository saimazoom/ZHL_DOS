# -*- coding: iso-8859-15 -*-
import sys
import getopt
import shutil
import itertools
import json

from collections import Counter

def usage():
    print "Dictionary Text compressor v0.1. Written by KMBR. ";
    print "Syntaxis: tc -i inputfile -o outputfile -c commentstyle -l compression -u";
    print " -i Path to the input file. ";
    print " -o Path to the desired output file.";
    print " -c Comment Style. Default character is !";       
    print " -l 0 PAWs compression histogram (fast). Default= 90 symbols.";
    print "    n Number of symbols. Optimal compression histogram (slow).";
    print " -u Use dictionary file";
def fileExists(f): 
    try:
        file = open(f);
    except IOError:
        exists = 0
    else:
        exists = 1
    return exists
        
def main(argv):
    
    extract = False;
    replace = False;
    inputfile = "";
    outputfile= "";
    minwords = 3;
    maxwords = 0;
    odelimiter = "\"";
    cdelimiter = "\"";
    commentstyle = "!";
    compression_level = 0;
    usedictionary = False;
    
    try:
        opts, args = getopt.getopt(argv,"hx:i:o:l:c:u",["help","input","output","level","comment","dictionary"]);
    except getopt.GetoptError, err:        
        usage();
        sys.exit(2);    

    for o, a in opts:            
        if o in ("-h", "--help"):
            usage();
            sys.exit();
        elif o=="-o":
            outputfile = a
        elif o=="-i":
            inputfile = a;
        elif o=="-c":
            print commentstyle;
            commentstyle=commentstyle.replace("!",a);
            print commentstyle;
        elif o=="-l":
            compression_level = int(a);
            if (compression_level==0):
                print "Compression level: PAWS algorithm for 96 symbols";
            else: 
                print "Optimal Compression level for ",compression_level," symbols"
        elif o=="-u":
            usedictionary = True;
        else:
            assert False, "unhandled option"
    
   # Comprueba que existe el fichero de entrada...
    if (fileExists(inputfile)==0):
        print "Error: Input file cannot be found.",inputfile;
        usage();
        sys.exit(2);
    if(outputfile==""):
        print "Error: Output file is missing.";
        usage();
        sys.exit(2);

    if (usedictionary==True):
        print "Using Dictionary file";
        if (fileExists("dictionary.txt")==0):
            print "Error: Dictionary file is missing (dictionary.txt)."
            sys.exit(2);
        
    file1 = inputfile;
    file2 = outputfile;

    inicio=0;
    final =0;
    caracter = "1";
    cadena = "";
    salir = 0;
    numchars=0;
    numstrings=0;
    actual_delimiter = "";
    ignore = 0;
    line_counter =0;

    print "Processing: ",inputfile;
  
    # Algoritmo de PAWs
    #1. Almacena las cadenas en un array
    print "Extracting strings";
    f1 = open(file1,'rb');
    content = f1.readlines()
    # Elimina las comillas de todas las cadenas y los espacios al comienzo/final.
    content = [w.replace('"', '') for w in content]
    content = [w.replace('\r', '') for w in content]
    content = [w.replace('\n', '') for w in content]
    # Sustituye las ? y los caracteres acentuados por los nuevos s?mbolos
    content = [w.replace('?', '#') for w in content]
    content = [w.replace('?', '$') for w in content]
    content = [w.replace('?', '%') for w in content]
    content = [w.replace('?', '&') for w in content]
    content = [w.replace('?', '\'') for w in content]
    content = [w.replace('?', '+') for w in content]
    content = [w.replace('?', '/') for w in content]
    content = [w.replace('?', '<') for w in content]

    
    print "Finished... ",len(content)," lines";
        
    #2. Crea las listas de caracteres para 2,3,4 y 5.
    print "Creating symbol list..."
    symbols_2=[];
    symbols_3=[];
    symbols_4=[];
    symbols_5=[];
    
    for line in content:
        # De 2 en 2 caracteres...
        symbols_2 = symbols_2 + ([line[i:i+2] for i in range(0, len(line)-2, 1)]);
            
        # De 3 en 3 caracteres...
        symbols_3 = symbols_3+ ([line[i:i+3] for i in range(0, len(line)-3, 1)]);
        
        # De 4 en 4 caracteres...
        symbols_4 = symbols_4 + ([line[i:i+4] for i in range(0, len(line)-4, 1)]);
       
        # De 5 en 5 caracteres...
        symbols_5 = symbols_5+ ([line[i:i+5] for i in range(0, len(line)-5, 1)]);

    # Convierte a arrays de 1 dimensi?n...
    print "... OK";
  
    #3. Calcula las frecuencias
    print "Creating frecuency tables..."
    symbols_2_freq = Counter (symbols_2);
    symbols_2_freq = symbols_2_freq.most_common();
    symbols_3_freq = Counter (symbols_3);
    symbols_3_freq = symbols_3_freq.most_common();
    symbols_4_freq = Counter (symbols_4);
    symbols_4_freq = symbols_4_freq.most_common();
    symbols_5_freq = Counter (symbols_5);
    symbols_5_freq = symbols_5_freq.most_common();
  
    # Busca la distribuci?n ?ptima de s?mbolos por fuerza bruta
    
    #3. Ordena de mayor a menor frecuencia.
    
    numAbrevTanteo = [ -1, -1, 0, 0, 0, 0 ]; # Histograma temporal 
    #bestnumAbrev = [ -1, -1, 70, 6, 6, 8 ]; # Histograma usado por PAWs, 90 s?mbolos
    bestnumAbrev = [ -1, -1, 86, 14, 19, 0]; # 42.06% , 86 14 19 0
    
    best_compression = 0;
    compressed_content=[];
       
    if (compression_level>0):       
        total_symbols = compression_level;
        permutations = list(itertools.product(range(0,total_symbols,1), repeat=4))
        
    else: # PAWS histogram
        permutations = [];
        permutations.append ( bestnumAbrev[2:6]);
        total_symbols = sum(bestnumAbrev[2:6]); # PAWs maximum number of symbols
    j=1;    
    
    for k in permutations:
        
        # S?lo tomamos las permutaciones cuyos elementos suman el n?mero m?ximo de s?mbolos
        if (sum(k)==total_symbols):
            compressed_content=[];
            numAbrevTanteo = [ -1, -1, k[0], k[1], k[2], k[3] ]; 
            symbol_list = [];

            if (usedictionary==False):
                symbol_list = symbols_5_freq[1:numAbrevTanteo[5]]+symbols_4_freq[1:numAbrevTanteo[4]] + symbols_3_freq[1:numAbrevTanteo[3]]+symbols_2_freq[1:numAbrevTanteo[2]]
            else:
                print "Loading Symbol List from dictionary"
                f = open('dictionary.txt', 'rb')
                symbol_list = json.load(f);    
                f.close();
               
                    
            #4. Reemplaza en las cadenas.
            count=0;  
            initial_size=0;
            total_size=0;
            for w in content:
                counter = 0;
                initial_size=initial_size+len(w);
                for i in symbol_list:
                    w = w.replace (i[:][0].encode('utf-8'),chr(128+counter)); # 128 + counter en binario
                    counter=counter+1
          
                total_size=total_size+len(w);
                compressed_content.append(w);
                count=count+1;
              
            # 5. Calcula el porcentaje de compresi?n  
            if (initial_size>0): 
                compression = float(100*(float(initial_size-total_size)/initial_size)); 
            
            #print ("%.2f %% compression, %u bytes out of %u bytes" %( compression, total_size,initial_size))
            if (compression>best_compression):
                bestnumAbrev[0:6]=numAbrevTanteo[0:6]
                best_compression = compression;
                #      if ( (j % 1000)==0):
                print "Iteration ",j, " of ",len(permutations), ", Best Compression so far: ", round(best_compression,2), "% for [ -1 -1 ",bestnumAbrev[2], " ",bestnumAbrev[3]," ",bestnumAbrev[4], " ", bestnumAbrev[5],"]"  
          
        j=j+1;
    
    print "Best compression %.2f %%" %best_compression    
    print "  for [ -1 -1 ",bestnumAbrev[2], " ",bestnumAbrev[3]," ",bestnumAbrev[4], " ", bestnumAbrev[5],"]"    
        
    #5. Genera la salida de texto comprimida
    f3 = open(outputfile,"wb"); 
    
    for item in compressed_content:
        f3.write("\"%s\"\n" % item)
    f3.close();
    
    print symbol_list
    f4 = open ("symbol_list.h","wb")    
    f4.write ("const unsigned char *symbol_list[] = {")
    counter=0;    
    for line in symbol_list:
        f4.write ("\""+line[0]+"\"");        
        counter=counter+1;
        if (counter<len(symbol_list)):
            f4.write(", ");
        f4.write ("// "+hex(128+counter-1)+"\n");
    
    f4.write ("};")
    f4.close();
    
    print len(content)," strings written."
    if (usedictionary==False):
        f = open('dictionary.txt', 'w');
        json.dump(symbol_list, f);
        f.close();
        
    print "Finished"
if __name__ == "__main__":
    main(sys.argv[1:])

