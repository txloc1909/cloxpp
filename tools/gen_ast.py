#!/usr/bin/python3

import sys


def define_type(file, basename, classname, fields):
    field_list = fields.split(', ')
    file.write('template<typename R>\n')
    file.write(f'struct {classname} : public {basename}<R> {{\n')

    # Fields
    for field in field_list:
        type_, name = field.split(' ')
        if type_ == basename:
            file.write(f'    {type_}<R> {name};\n')
        else:
            file.write(f'    {type_} {name};\n')
    file.write('\n')

    # Constructor
    file.write(f'    {classname} (')
    for i, field in enumerate(field_list):
        type_, name = field.split(' ')
        if type_ == basename:
            file.write(f'{type_}<R> {name}')
        else:
            file.write(f'{type_} {name}')

        if i < len(field_list) - 1:
            file.write(', ')

    file.write(') {\n')
    for field in field_list:
        name = field.split(' ')[1]
        file.write(f'        this->{name} = {name};\n')
    file.write('    }\n\n')

    # Accept visitor
    file.write(f'    R accept({basename}Visitor<R> visitor) override {{\n')
    file.write('        return visitor.visit(this);\n')
    file.write('    }\n')

    file.write('};\n')


def define_visitor(file, basename, types):
    file.write('template<typename R>\n')
    file.write(f'class {basename}Visitor {{\n')

    for type_ in types:
        classname = type_.split(':')[0].strip()
        file.write(
            f'    virtual R visit({classname}<R>* {basename.lower()}) = 0;\n')

    file.write('};\n')


def define_ast(output_dir, basename, types):
    path = f"{output_dir}/{basename.lower()}.hpp"

    with open(path, "w", encoding="utf-8") as f:
        # f.write('#include "literal.hpp"\n')
        # f.write('#include "token.hpp"\n')
        f.write('typedef struct Literal {} Literal;\n')
        f.write('typedef struct Token {} Token;\n')
        f.write('\n')

        # Forward-declare all AST types
        for type_ in types:
            classname = type_.split(':')[0].strip()
            f.write('template<typename R>\n')
            f.write(f'struct {classname};\n')
            f.write('\n')

        define_visitor(f, basename, types)
        f.write('\n')

        f.write('template<typename R>\n')
        f.write(f'struct {basename} {{\n')
        f.write(f'    virtual R accept({basename}Visitor<R> visitor) = 0;\n')
        f.write('};\n\n')

        for type_ in types:
            classname = type_.split(':')[0].strip()
            fields = type_.split(':')[1].strip()
            define_type(f, basename, classname, fields)
            f.write('\n')


def main(args):
    if len(args) != 2:
        print(f"Usage: {args[0]} <output directory>")
        sys.exit(64)

    output_dir = args[1]
    define_ast(output_dir, "Expr", [
        "BinaryExpr:    Expr left, Token op, Expr right",
        "GroupingExpr:  Expr expression",
        "LiteralExpr:   Literal value",
        "UnaryExpr:     Token op, Expr right",
    ])


if __name__ == "__main__":
    main(sys.argv)
