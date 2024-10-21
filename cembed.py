import os, re

def fmt(fmt, opt):
    for k in opt:
        fmt = fmt.replace('{' + k + '}', str(opt[k]))
    return fmt


def make_array(data):
    i = [0]

    def fn(x):
        x = str(x) + ','
        if i[0] + len(x) > 78:
            i[0] = len(x)
            x = '\n' + x
        else:
            i[0] += len(x)
        return x

    return '{' + ''.join(map(fn, data)).rstrip(',') + '}'


def safename(filename):
    return re.sub(r'[^a-z0-9]', '_', os.path.basename(filename).lower())


def process(filename):
    strings = []

    with open(filename, 'rb') as f:
        data = f.read()
        strings.append(
            fmt(
                '/* {filename} */\n'
                'static const char {name}[] = \n{array};',
                {
                    'filename': os.path.basename(filename),
                    'name': safename(filename),
                    'array': make_array(data)
                }
            )
        )

    return "/* Automatically generated; do not edit */\n\n" + "\n\n".join(strings)


def main():
    import sys

    if len(sys.argv) < 2:
        print("usage: embed filename")
        sys.exit(1)

    print(process(sys.argv[1]))


if __name__ == "__main__":
    main()