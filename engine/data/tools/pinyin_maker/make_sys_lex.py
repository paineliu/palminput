import os

def load_lab_file(filename):
    print(filename)
    set_phrase = set()
    f = open(filename, 'r', encoding='utf-8')
    for each in f:
        phrase = each.strip()
        set_phrase.add(phrase)
    f.close()

    return set_phrase

def load_file(lab_filename, filename, out_filename, max_phrase_num):
    set_lab = load_lab_file(lab_filename)
    out_phrase_total = 0
    print(filename)
    f_o = open(out_filename, 'w', encoding='utf-8')
    f = open(filename, 'r', encoding='utf-8')
    for each in f:
        items = each.split()
        if ("'" not in items[1]):
            f_o.write(each)
        elif out_phrase_total < max_phrase_num and items[0] in set_lab:
            f_o.write(each)
            out_phrase_total += 1
    f.close()
    f_o.close()


load_file('../phrase/tencent_lab.txt', '../phrase/core_phrase.txt','../phrase/sys_lex.txt', 60000)