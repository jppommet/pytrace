import urwid
from db import DB

class Selectable(urwid.FlowWidget):
    def selectable(self):
        return True
    def keypress(self, size, key):
        pass

def get_data():
    return list(DB().fetch_pretty())
    
data = get_data()
palette = [('header', 'white', 'black'),
           ('reveal focus', 'black', 'dark cyan', 'standout'),
           ('time', '', '', '', '#9cf', ''),
           ('tid', '', '', '', '#f99', ''),
           ('module', '', '', '', '#9fc', ''),
           ('func', '', '', '', '#9cf', ''),
           ('name', '', '', '', '#9fc', ''),
           ('type', '', '', '', '#9ff', ''),
           ('value', '', '', '', '#f99', '')]
content = urwid.SimpleListWalker(map(urwid.Text, data))
listbox = urwid.ListBox(content)
show_key = urwid.Text(u"", wrap='clip')
head = urwid.AttrMap(show_key, 'header')
top = urwid.Frame(listbox, head)

def show_all_input(input, raw):
    show_key.set_text(u"Pressed: " + u" ".join([
        unicode(i) for i in input]))
    return input

def exit_on_cr(input):
    if input == "G":
        content.set_focus(len(data) - 1)
    elif input == "p":
        content.set_focus(0)
    elif input == "T":
        new_data = get_data()
        if len(new_data) != len(data):
            for i in new_data[-(len(new_data)-len(data)):]:
                content.append(urwid.Text([i]))
            content.set_focus(len(new_data) - 1)
    elif input == 'enter':
        raise urwid.ExitMainLoop()

loop = urwid.MainLoop(top, palette, input_filter=show_all_input, unhandled_input=exit_on_cr)
loop.screen.set_terminal_properties(colors=256)
loop.run()