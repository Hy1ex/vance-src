import gdb
from gdb.printing import PrettyPrinter 

"""
Simple command to show the cursor when debugging source
When the game crashes, the cursor will not be shown automatically.
"""
class SDL2ShowCursor(gdb.Command):
    def __init__(self):
        super(SDL2ShowCursor, self).__init__("sdl2-ungrab-cursor", gdb.COMMAND_USER)
        
    def invoke(self, arg, from_tty):
        # I have literally no clue what is necessary or not
        try: 
            gdb.execute("call (int)SDL_ShowCursor(1)")
            gdb.execute("call (void)SDL_SetWindowGrab(g_SDLWindow, SDL_FALSE)")
            gdb.execute("call (int)SDL_SetRelativeMouseMode(0)")
        except: 
            pass
    
SDL2ShowCursor()
