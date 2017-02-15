#!/usr/bin/env python

import sys
import toml
import json
from tabulate import tabulate

field_names = ['pos', 'size', 'name', 'value', 'desc']
field_title = ['Position', 'Size', 'Name', 'Value', 'Description']


common_fields = [
    {'name': 'CRC', 'size': 2, 'desc': 'Cyclic redundancy check'},
    {'name': 'Command number'},
    {'name': 'Packet size', 'desc': 'Number of bytes, excluding the header'},
]
stream_fields = [
    {'name': 'Init byte', 'value':'0x7e'},
    {'name': 'Unused', 'size': 2},
    {'name': 'Command number'},
    {'name': 'Packet size', 'desc': 'Number of bytes, excluding the header'},
]


def title(text, char='-'):
    return '\n%s\n%s\n' % (text, char*len(text))


def fields_table(fields, number, is_stream=False):
    """Build a table from a list of fields."""
    pos = 0
    if is_stream:
        fields = stream_fields + fields
        number_field = fields[2]
        size_field = fields[3]
        header_size = 5
    else:
        fields = common_fields + fields
        number_field = fields[1]
        size_field = fields[2]
        header_size = 4

    for field in fields:
        field['pos'] = pos
        field['size'] = field.get('size', 1)
        pos += field['size']

    number_field['value'] = number     # fill command number
    size_field['value'] = pos - header_size    # fill packet size

    table = [[t.get(f) for f in field_names] for t in fields]
    return tabulate(table, field_title, tablefmt='rst')


def print_fields(fields, title, number, is_stream=False):
    print "\n**%s:**\n" % title
    if type(fields) in (str, unicode):
        print fields
    else:
        print fields_table(fields, number, is_stream)

def print_command(cmd, name):
    print "\n.. _%s:" % name
    print title(name)
    print cmd['desc']
    number = cmd['number']
    st = cmd.get('stream', False)

    if cmd.get('rw'):
        cmd_rd = cmd['command'][:]
        cmd_rd.pop()
        print_fields(cmd_rd, "Command (read)", number, st)
        print_fields(cmd['command'], "Command (write)", number, st)
        print_fields(cmd['command'], "Response", number, st)
    else:
        if cmd.has_key('command'):
            print_fields(cmd['command'], "Command", number, st)
        if cmd.has_key('response'):
            print_fields(cmd['response'], "Response", number, st)


def command_list(commands):
    table = [[k+'_', commands[k]['number'], commands[k]['desc']]
             for k in sorted(commands)]
    return tabulate(table, ['Name', 'Number', 'Description'], tablefmt='rst')


if __name__ == '__main__':
    filename = sys.argv[1]
    commands = toml.load(open(filename))

    print title("Serial commands", '=')

    print ".. _`List of commands`:\n"
    print title("List of commands")
    print command_list(commands)

    for name in sorted(commands):
        cmd = commands[name]
        print_command(cmd, name)
