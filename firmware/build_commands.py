
class protocol(object):
    """ pin map is a dict that has ON and OFF command for an indexed pin  
        LinuxCNC only gives you 4 digital outs.  

        Example:
            pc = protocol()
            pc.map_pin(0,['M64 P0', 'M65 P0'])
            pc.map_pin(1,['M64 P1', 'M65 P1'])
            pc.map_pin(2,['M64 P2', 'M65 P2'])
            pc.map_pin(3,['M64 P3', 'M65 P3'])
            pc.add_command('linethick', 0xa )
            pc.add_command('headup'   , 0x1 )
            pc.add_command('headdown' , 0x2 )
            print(pc.pinmap)
            pc.show_commands()

    """
    
    def __init__(self):
        self.com  = {}                # name byte(command)   
        self.pmap = {}                # int, gcodeON gcodeOFF
        self.trigger  = ['M7', 'M9']  # coolant mist (falling edge) as a trigger to load 

    def map_pin(self, pm_key, pm_val):
        """define a pin and specify ON and OFF command in Gcode   
        """
        self.pmap[pm_key] = pm_val

    def add_command(self, name, bytecom ):
        self.com[name] = bytecom
    
    def show_commands(self):
        for key, value in self.com.items():
            if value<255:
                print(key+" "+ '0x%s'%bytearray([value]).hex())
            if value>=256 and value < 1023:
                print(hex(((abs(x) ^ 0xffff) + 1) & 0xffff))

    def exists(self, name):
        if name in self.com:
            return self.com[name] 
        else:
            return False    


#######--------#######--------#######--------#######
#######--------#######--------#######--------#######

class gen_gcode(object):
    """
        test of system to embed encoded bytes in gcode 
    """

    def __init__(self, pinmap):
        self.DEBUG = False

        self.pm = pinmap
        self.commands = []
        self.gcode    = []
        
    def add(self, name, value):
        com = self.pm.exists(name)
        if com:
            self.commands.append([ name, com, value ])
            print('added new command: %s %s'%(name,value))
        else:
            print('command %s does not exist in protocol'%name)

    def serialize_word(self, int_com):
        #serialize half bytes of data into G-code 
        self.gcode.append(self.pm.trigger[0])
        for i in range(3,-1,-1):
            if self.DEBUG:
                if(int_com>>i&0x1):
                    print('BIT ON    -', self.pm.pmap[i][0])
                else:
                    print('BIT OFF   -', self.pm.pmap[i][1])    
            if(int_com>>i & 0x1):
                self.gcode.append(self.pm.pmap[i][0])
            else:
                self.gcode.append(self.pm.pmap[i][1])
        self.gcode.append(self.pm.trigger[1])  

    def serialize_8bit(self, int_com):
        #serialize a single byte of data into G-code 
        for i in [4,0]:
            self.serialize_word(int_com>>i&0x0f)

    def serialize_16bit(self, int_com):
        #serialize 2 bytes of data into G-code 
        for i in [8,0]:
            self.serialize_word(int_com>>i & 0xff)

    def export(self, filename):
        for c in self.commands:
            comstr=  ''
            com_int = self.pm.com[c[0]]
            self.gcode.append(';%s : 0x%s'%(c[0],bytearray([com_int]).hex()))
            
            #self.serialize_word(com_int)
            self.serialize_8bit(com_int)
            #self.serialize_16bit(com_int)
            
            if self.DEBUG:
                print('TRIGGER up-', self.pm.trigger[0])    
                print('TRIGGER dn-', self.pm.trigger[1])   
            self.gcode.append('')
                
        f = open(filename, "w")
        for l in self.gcode:
            f.write('%s\n'%l)
        
        #end the program with M2
        f.write(';prog end\n')
        f.write('M2\n')
        f.close()
    


#######--------#######--------#######--------#######
#######--------#######--------#######--------#######
"""
#modes:
1 dot  
2 dispense 
3 smear 
4 brush (Z rotation)
#toolchanger:
1 small brush 
2 large brush 
3 pallate knife 
"""

pc = protocol()
pc.map_pin(0,['M64 P0', 'M65 P0'])
pc.map_pin(1,['M64 P1', 'M65 P1'])
pc.map_pin(2,['M64 P2', 'M65 P2'])
pc.map_pin(3,['M64 P3', 'M65 P3'])
pc.add_command('soft_reset' , 0x01 )
pc.add_command('dwell'      , 0x02 )
pc.add_command('head_up'    , 0x04 )
pc.add_command('head_dwn'   , 0x06 )
pc.add_command('pmp_on'     , 0x08 )
pc.add_command('pmp_off'    , 0x0a )
pc.add_command('pmp_rev'    , 0x0c )
pc.add_command('z_offset'   , 0x0e )


#print(pc.pinmap)
#pc.show_commands()


#######--------#######--------#######--------#######
#######--------#######--------#######--------#######

gc = gen_gcode(pinmap=pc)
gc.add('head_up'  , 0 )
gc.add('head_dwn' , 0 )
gc.export('test_head.ngc')
 
gc = gen_gcode(pinmap=pc)
gc.add('pmp_on'  , 0 )
gc.add('pmp_off' , 0 )
gc.export('test_pump.ngc') 






