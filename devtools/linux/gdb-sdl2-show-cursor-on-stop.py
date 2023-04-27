import gdb
from gdb.printing import PrettyPrinter 

"""
Hook up the "stop" event
"""
def stop_handler(event):
    gdb.execute("sdl2-ungrab-cursor")
    
gdb.events.stop.connect(stop_handler)
