# coding=utf-8

import sys
import os

class PinyinData(object):

    def __init__(self):
        self.shengmu = ''
        self.yunmu = ''

    def __repr__(self): 
        return "PinyinData()"

    def __str__(self): 
        return self.shengmu + ' ' + self.yunmu


class PinyinInfo(object):

    def __init__(self, shengmu_filename, yunmu_filename, pinyin_filename, group_filename):

        self.shengmu_list = []
        self.yunmu_list = []
        self.pinyin_list = []
        self.pinyin_group_set = set()
        self.pinyin_group = {}
        self.pinyin_max_len = 0
    
        self.shengmu_list.append('')
        self.yunmu_list.append('')

        self.pinyin_list.append('')
        self.pinyin_list.append("'")
        self.pinyin_list.append("^")
        for i in range(0, 10):
            self.pinyin_list.append(chr(ord('0') + i))
        for i in range(0, 26):
            self.pinyin_list.append(chr(ord('A') + i))

        self.__loadFile(self.shengmu_list, shengmu_filename)
        self.__loadFile(self.yunmu_list, yunmu_filename)
        self.__loadFile(self.pinyin_list, pinyin_filename)
        self.__loadGroupFile(self.pinyin_group, group_filename)
        self.shengmu_list = sorted(self.shengmu_list)
        self.yunmu_list = sorted(self.yunmu_list)
        self.pinyin_list = sorted(self.pinyin_list)
        for each in self.pinyin_group:
            self.pinyin_group_set.add(self.pinyin_group[each])

        for each_item in self.pinyin_list:
            if self.pinyin_max_len < len(each_item):
                self.pinyin_max_len = len(each_item)

    def __loadFile(self, obj, filename):
        print('load    ' + filename, os.path.abspath(filename))
        
        f = open(filename, 'r')
        for each in f:
            if each[0] != ';':
                items = each.split()
                if len(items) > 0 and items[0] not in obj:
                    obj.append(items[0])
        f.close()

    def __loadGroupFile(self, obj, filename):
        print('load    ' + filename)
        f = open(filename, 'r')
        for each in f:
            if each[0] != ';':
                items = each.split()
                if len(items) == 3:
                    obj[items[1]] = int(items[0])

        f.close()

    def isShengmu(self, val):
        return val in self.shengmu_list
    
    def isYunmu(self, val):
        return val in self.yunmu_list
    
    def isSplit(self, val):
        return val == "'"

    def getPinyinMaxLen(self):
        return self.pinyin_max_len

    def getPinyinGroup(self, val):
        if val in self.pinyin_group:
            return self.pinyin_group[val]
        return 0

    def getPinyinGroupSize(self):
        return len(self.pinyin_group_set)

    def getPinyinIndex(self, val):
        return self.pinyin_list.index(val)

    def getPinyinStr(self, index):
        return self.pinyin_list[index]

    def getPinyinList(self):
        return self.pinyin_list

    def getJianpinList(self):
        return self.shengmu_list

    def getPinyinListByJianpin(self, jianpin):
        pinyin_list = []
        for item in self.getPinyinList():
            pinyin_data = self.splitPinyin(item)
            if jianpin == pinyin_data.shengmu:
                pinyin_list.append(item)
        return pinyin_list

    def splitPinyin(self, pinyin):

        pinyin_data = PinyinData()

        # zero-onset
        if self.isYunmu(pinyin):
            pinyin_data.yunmu = pinyin

        # zh, ch, sh
        if (len(pinyin) > 2) and self.isShengmu(pinyin[:2]) and self.isYunmu(pinyin[2:]):
            pinyin_data.shengmu = pinyin[:2]
            pinyin_data.yunmu = pinyin[2:]

        # others
        if (len(pinyin) > 1) and self.isShengmu(pinyin[:1]) and self.isYunmu(pinyin[1:]):
            pinyin_data.shengmu = pinyin[:1]
            pinyin_data.yunmu = pinyin[1:]

        return pinyin_data


class HanData:

    def __init__(self):
        self.id = 0           # id order
        self.uid = 0          # unicode << 16 | pinyin_id
        self.han_unicode = 0  # unicode value
        self.han_utf8 = ''    # unicode string
        self.pinyin_id = 0
        self.pinyin_str = ''
        self.freq = 0
    
    def __repr__(self): 
        return "HanData()"

    def __str__(self): 
        return self.han_utf8 + ' ' + self.pinyin_str

    def __lt__(self, other): 
        return self.uid < other.uid 


class HanInfo(object):

    def __init__(self, pinyin_info, han_pinyin_filename):

        self.han_pinyin_list = []
        self.han_pinyin_list_by_asc_order = []
        self.han_pinyin_dict = {}
        self.pinyin_info = pinyin_info

        self.__loadFile(pinyin_info, han_pinyin_filename)

        self.han_pinyin_list_by_asc_order = sorted(self.han_pinyin_list)

        for eachItem in self.han_pinyin_list:
            if eachItem.han_unicode != 0:
                self.han_pinyin_dict[(eachItem.han_unicode << 16) | eachItem.pinyin_id] = eachItem.id

    def __loadFile(self, pinyin_info, filename):

        print('load    ' + filename)
        
        self.han_pinyin_list.append(HanData())
        
        f = open(filename, 'r', encoding='utf-8')
        han_id = 1
        for each_line in f:
            try:
                if each_line[0] != ';':
                    items = each_line.strip().split()
                    if len(items) > 0:
                        uni = items[0]
                        han_data = HanData()
                        han_data.id = han_id
                        han_data.han_unicode = ord(uni)
                        han_data.han_utf8 = items[0]
                        han_data.pinyin_id = 0
                        han_data.pinyin_str = ''
                        han_data.freq = 0

                        if len(items) > 1:
                            han_data.freq = int(items[-1])

                        if len(items) > 2:
                            han_data.pinyin_id = pinyin_info.getPinyinIndex(items[1])
                            han_data.pinyin_str = items[1]

                        han_data.uid = (han_data.han_unicode << 16) | han_data.pinyin_id

                        self.han_pinyin_list.append(han_data)

                        han_id = han_id + 1
            except:
                print('han_code err: ' + each_line.strip())

        f.close()

    def getHanList(self):
        return self.han_pinyin_list

    def getHanListAscOrder(self):
        return self.han_pinyin_list_by_asc_order

    def getHanIdByPinyinId(self, han_str, pinyin_id):
        if len(han_str) == 0:
            return 0
        han_unicode = ord(han_str)
        id = (han_unicode << 16) + pinyin_id
        if id in self.han_pinyin_dict:
            return self.han_pinyin_dict[id]
        return 0

    def getHanIdByPinyinStr(self, han_str, pinyin_str):
        pinyin_id = self.pinyin_info.getPinyinIndex(pinyin_str)
        return self.getHanIdByPinyinId(han_str, pinyin_id)


class FuzzyData(object):

    def __init__(self):
        self.fuzzy_id = 0
        self.fuzzy_macro = ''
        self.fuzzy_type = ''
        self.src_pinyin = ''
        self.dst_pinyin = ''
   
    def __repr__(self): 
        return "FuzzyData()"

    def __str__(self): 
        return str(self.fuzzy_id) + ' ' + self.fuzzy_macro + ' ' + self.fuzzy_type + ' ' + self.src_pinyin + ' ' + self.dst_pinyin


class FuzzyMacro(object):

    def __init__(self):
        self.fuzzy_id = ''
        self.fuzzy_macro = ''

    def __repr__(self): 
        return "FuzzyMacro()"

    def __str__(self): 
        return self.fuzzy_id + ' ' + self.fuzzy_macro

    def __eq__(self, other): 
        return self.fuzzy_id == other.fuzzy_id

    def __lt__(self, other): 
        return self.fuzzy_id < other.fuzzy_id 

class FuzzyInfo(object):
    
    def __init__(self, pinyin_info, fuzzy_filename):
        
        self.macro_list = []
        self.fuzzy_list = []

        self.pinyin_info = pinyin_info

        self.__loadFile(fuzzy_filename)

    def __loadFile(self, filename):
        
        print('load    ' + filename)

        f = open(filename, 'r', encoding='utf-8')
        
        for eachline in f:
            try:
                if eachline[0] != ';':
                    items = eachline.split()
                    if len(items) == 5:
                        fuzzy_data = FuzzyData()
                        fuzzy_data.fuzzy_id = int(items[0], 16)
                        fuzzy_data.fuzzy_macro = items[1]
                        fuzzy_data.fuzzy_type = items[2]
                        fuzzy_data.src_pinyin = items[3]
                        fuzzy_data.dst_pinyin = items[4]
                        if fuzzy_data not in self.fuzzy_list:
                            self.fuzzy_list.append(fuzzy_data)
                        else:
                            print(eachline)

                        fuzzy_macro = FuzzyMacro()
                        fuzzy_macro.fuzzy_id = items[0]
                        fuzzy_macro.fuzzy_macro = items[1]
                        if fuzzy_macro not in self.macro_list:
                            self.macro_list.append(fuzzy_macro)
            except:
                print(eachline)
        f.close()

        self.macro_list = sorted(self.macro_list)

    def getFuzzyMacroList(self):
        return self.macro_list
   
    def getFuzzyMacroByFuzzyId(self, fuzzy_id):
        fuzzy_macro = ""
        for eachItem in self.fuzzy_list:
            if eachItem.fuzzy_id & fuzzy_id:
                if fuzzy_macro.find(eachItem.fuzzy_macro) < 0:
                    fuzzy_macro += eachItem.fuzzy_macro + "&"
        if len(fuzzy_macro) > 0:
            fuzzy_macro = fuzzy_macro[:-1]
        return fuzzy_macro

    def getFuzzyIdList(self, fuzzy_type):
        fuzzyIdSet = set()
        for eachItem in self.fuzzy_list:
            if eachItem.fuzzy_type == fuzzy_type:
                fuzzyIdSet.add(eachItem.fuzzy_id)
        return list(fuzzyIdSet)

    def getFuzzyItemList(self, fuzzy_type, pinyin):
        fuzzy_data_list = []
        for eachItem in self.fuzzy_list:
            if eachItem.fuzzy_type == fuzzy_type and eachItem.dst_pinyin == pinyin:
                fuzzy_data_list.append(eachItem)
        return fuzzy_data_list


class EncodeData(object):

    def __init__(self):
        self.node_str = ""
        self.node_type = 0
        self.fuzzy_id = 0
        self.fuzzy_str = ""
        self.pinyin_list = []

    def __repr__(self): 
        return "EncodeData()"

    def __str__(self): 
        ret_str = self.fuzzy_str + ":"
        for eachItem in self.pinyin_list:
            ret_str += eachItem + '|'
        return ret_str[:-1]

    def __hash__(self): 
        return hash(self.node_str)

    def __eq__(self, other): 
        return self.node_str == other.node_str


class EncodeInfo(object):

    def __init__(self, pinyin_info, fuzzy_info):

        self.NODE_PINYIN = 1
        self.NODE_JIANPIN = 2
        self.NODE_BUQUAN = 4
        self.NODE_SPLIT = 8
        self.NODE_LETTER = 16

        self.node_pinyin_dict = {}
        self.node_pinyin_list = []

        self.pinyin_info = pinyin_info
        self.fuzzy_info = fuzzy_info

        self.node_pinyin_dict[''] = []

        self.__addNodeJianpin()
        self.__addNodePinyin()
        self.__addNodeIncomp()

        node_max_len = self.pinyin_info.getPinyinMaxLen()

        for eachItem in self.node_pinyin_dict:
            self.node_pinyin_list.append(' ' * (node_max_len - len(eachItem)) + eachItem)

        self.node_pinyin_list = sorted(self.node_pinyin_list)

        for i in range(len(self.node_pinyin_list)):
            self.node_pinyin_list[i] = self.node_pinyin_list[i].strip()

    def getNodeDict(self):
        return self.node_pinyin_dict

    def getNodeList(self):
        return self.node_pinyin_list

    def __addNodePinyin(self):
        for item in self.pinyin_info.getPinyinList():
            if len(item) > 0:
                self.__addNodePinyinItem(item)

    def __addNodeJianpin(self):
        for item in self.pinyin_info.getJianpinList():
            if len(item) > 0:
                self.__addNodeJianpinItem(item)

    def printNodePinyin(self):
        for eachNode in self.node_pinyin_list:
            
            print(eachNode, end=" ")
            for itemPinyin in self.node_pinyin_dict[eachNode]:
                print(itemPinyin, end=" ")
            print("")

    def __isNodeValid(self, node_str, fuzzy_id):
        if node_str in self.node_pinyin_dict:
            for eachItem in self.node_pinyin_dict[node_str]:
                if eachItem.fuzzy_id == 0 or eachItem.fuzzy_id == fuzzy_id:
                    return True
        return False

    def __addNodeIncomp(self):
        
        list_pinyin = sorted(self.node_pinyin_dict)

        incomp_node_list = []
        for eachNode in list_pinyin:
            if len(eachNode) > 2:
                if self.__isNodeValid(eachNode, 0):
                    for i in range(len(eachNode) -1, 1, -1):
                        subNode = eachNode[:i]
                        if not self.__isNodeValid(subNode, 0):
                            incomp_node_list.append([subNode, eachNode])
                        else:
                            break
        
        for eachNode in incomp_node_list:
            self.__addNodeItem(self.NODE_BUQUAN, eachNode[0], eachNode[1], 0)

    def __addNodePinyinItem(self, pinyin):

        if self.pinyin_info.isSplit(pinyin):
            self.__addNodeItem(self.NODE_SPLIT, pinyin, pinyin, 0)
            return

        if pinyin.isdigit():
            self.__addNodeItem(self.NODE_LETTER, pinyin, pinyin, 0)
            return

        if pinyin.isupper():
            self.__addNodeItem(self.NODE_LETTER, pinyin, pinyin, 0)
            return

        pyItem = self.pinyin_info.splitPinyin(pinyin)
        
        fuzzy_quanpin_list = self.fuzzy_info.getFuzzyItemList("qp", pinyin)
        fuzzy_shengmu_list = self.fuzzy_info.getFuzzyItemList("sm", pyItem.shengmu)
        fuzzy_yunmu_list = self.fuzzy_info.getFuzzyItemList("ym", pyItem.yunmu)

        self.__addNodeItem(self.NODE_PINYIN, pinyin, pinyin, 0)

        if (len(fuzzy_quanpin_list) > 0):
            for eachItem in fuzzy_quanpin_list:
                self.__addNodeItem(self.NODE_PINYIN, eachItem.src_pinyin, pinyin, eachItem.fuzzy_id)

        if (len(fuzzy_shengmu_list) > 0):
            for eachItem in fuzzy_shengmu_list:
                node_str = eachItem.src_pinyin + pyItem.yunmu
                self.__addNodeItem(self.NODE_PINYIN, node_str, pinyin, eachItem.fuzzy_id)

        if (len(fuzzy_yunmu_list) > 0):
            for eachItem in fuzzy_yunmu_list:
                node_str = pyItem.shengmu + eachItem.src_pinyin
                self.__addNodeItem(self.NODE_PINYIN, node_str, pinyin, eachItem.fuzzy_id)

        if (fuzzy_shengmu_list and fuzzy_yunmu_list):
            for eachShengmu in fuzzy_shengmu_list:
                for eachYunmu in fuzzy_yunmu_list:
                    node_str = eachShengmu.src_pinyin + eachYunmu.src_pinyin
                    self.__addNodeItem(self.NODE_PINYIN, node_str, pinyin, eachShengmu.fuzzy_id | eachYunmu.fuzzy_id)

    def __addNodeJianpinItem(self, jianpin):

        for eachPinyin in self.pinyin_info.getPinyinListByJianpin(jianpin):
            self.__addNodeItem(self.NODE_JIANPIN, jianpin, eachPinyin, 0)

        fuzzy_jianpin_list = self.fuzzy_info.getFuzzyItemList("jp", jianpin)
        for fuzzyItem in fuzzy_jianpin_list:
            for eachPinyin in self.pinyin_info.getPinyinListByJianpin(fuzzyItem.src_pinyin):
                self.__addNodeItem(self.NODE_JIANPIN, jianpin, eachPinyin, fuzzyItem.fuzzy_id)

    def __addNodeItem(self, node_type, node_str, pinyin_str, fuzzy_id):
        
        #print(node_type, node_str, pinyin_str, fuzzy_id)

        if (not node_str in self.node_pinyin_dict):
            self.node_pinyin_dict[node_str] = []

        find_same = False
        for eachItem in self.node_pinyin_dict[node_str]:
            if (eachItem.fuzzy_id == fuzzy_id):
                eachItem.pinyin_list.append(pinyin_str)
                find_same = True
                break

        if (not find_same):
            nodePinyinItem = EncodeData()
            nodePinyinItem.node_type = node_type
            nodePinyinItem.node_str = node_str
            nodePinyinItem.fuzzy_id = fuzzy_id
            nodePinyinItem.fuzzy_str = self.fuzzy_info.getFuzzyMacroByFuzzyId(fuzzy_id)
            nodePinyinItem.pinyin_list.append(pinyin_str)

            self.node_pinyin_dict[node_str].append(nodePinyinItem)

class KeyInfo(object):

    def __init__(self, key_filename):

        self.key_dict = {}
        self.key_list = []

        self.__loadFile(key_filename)

    def __loadFile(self, filename):

        print('load    ' + filename)

        f = open(filename, 'r', encoding='utf-8')
        for eachline in f:
            eachline = eachline.strip()
            try:
                if len(eachline) > 0 and eachline[0] != ';':
                    items = eachline.split()
                    if (len(items) == 2):
                        if (items[0] in self.key_dict):
                            self.key_dict[items[0]].append(items[1])
                        else:
                            code_item = []
                            code_item.append(items[1])
                            self.key_dict[items[0]] = code_item

            except:
                print(eachline)
        f.close()

        self.key_list = sorted(self.key_dict)
        
        for eachItem in self.key_dict:
            self.key_dict[eachItem] = sorted(self.key_dict[eachItem])

    def getKeyList(self):
        return self.key_list

    def getKeyDict(self):
        return self.key_dict


def writeFuzzyHxxFile(fuzzy_hxx_filename, pinyin_info, fuzzy_info, node_info, key_info, han_info):

    print('write   ' + fuzzy_hxx_filename)
    f = open(fuzzy_hxx_filename, 'w')

    f.write('#ifndef __PY_CONST_H__\n')
    f.write('#define __PY_CONST_H__\n')
    f.write('\n')

    fuzzy_macro_max_len = 0

    for eachItem in fuzzy_info.macro_list:
        if fuzzy_macro_max_len < len(eachItem.fuzzy_macro):
           fuzzy_macro_max_len = len(eachItem.fuzzy_macro)

    for eachItem in fuzzy_info.macro_list:
        f.write('#define ' + eachItem.fuzzy_macro + ' ' * (fuzzy_macro_max_len + 4 - len(eachItem.fuzzy_macro)) + eachItem.fuzzy_id + '\n')

    f.write('\n')

    node_pinyin_total = 0
    nodeDict = node_info.getNodeDict()
    for eachNode in node_info.getNodeList():
        for eachItem in nodeDict[eachNode]:
            node_pinyin_total += len(eachItem.pinyin_list)

    f.write('#define PY_PINYIN_TOTAL           %d\n' % len(pinyin_info.getPinyinList()))
    f.write('#define PY_PINYIN_MAX_LEN         %d\n' % pinyin_info.getPinyinMaxLen())
    f.write('#define PY_HAN_TOTAL              %d\n' % (len(han_info.getHanList())))
    f.write('#define PY_NODE_TOTAL             %d\n' % len(node_info.getNodeList()))
    f.write('#define PY_NODE_PINYIN_TOTAL      %d\n' % node_pinyin_total)
    f.write('\n')
    f.write('#define PY_NODE_TYPE_QUANPIN      0x%02x\n' % 1)
    f.write('#define PY_NODE_TYPE_JIANPIN      0x%02x\n' % 2)
    f.write('#define PY_NODE_TYPE_BUQUAN       0x%02x\n' % 4)
    f.write('#define PY_NODE_TYPE_SPLIT        0x%02x\n' % 8)
    f.write('#define PY_NODE_TYPE_LETTER       0x%02x\n' % 16)

    f.write('\n#endif\n')
    
    f.close()


def writePinyinHxxFile(pinyin_hxx_filename):
    
    print('write   ' + pinyin_hxx_filename)

    f = open(pinyin_hxx_filename, 'w')

    f.write('#ifndef __PY_HAN_DATA_H__\n')
    f.write('#define __PY_HAN_DATA_H__\n')
    f.write('\n')
    f.write('#include "PYHanDef.h"\n')
    f.write('\n')
    f.write('#ifdef __cplusplus\n')
    f.write('extern "C" {\n')
    f.write('#endif /* __cplusplus */\n')
    f.write('\n')
    f.write('const PY_HAN_DATA * PYHan_GetHanData();\n')
    f.write('\n')
    f.write('#ifdef __cplusplus\n')
    f.write('}\n')
    f.write('#endif /* __cplusplus */\n')
    f.write('\n')
    f.write('#endif\n')
    
    f.close()

def writePinyinCxxFile(pinyin_cxx_filename, pinyin_info, fuzzy_info, node_info, key_info, han_info):

    print('write   ' + pinyin_cxx_filename)

    f = open(pinyin_cxx_filename, 'w', encoding='utf-8')

    f.write('#include "PYHanData.h"\n')
    f.write('\n')

    # pinyin item

    f.write('const static PY_PINYIN_ITEM G_PY_PINYIN_ITEM_DAT[' + str(len(pinyin_info.getPinyinList())) + '] =\n')
    f.write('{\n')

    data_offset = 0
    for eachItem in pinyin_info.getPinyinList():
        f.write('    0x%04x, ' % data_offset + '0x%02x, ' % len(eachItem) + '0x%02x, ' % pinyin_info.getPinyinGroup(eachItem) + '/* ' + eachItem + ' */\n')
        data_offset += len(eachItem) + 1

    f.write('};\n')
    f.write('\n')

    #pinyin string

    pinyin_data_size = data_offset

    f.write('const static PY_CHAR G_PY_PINYIN_STR_DAT[' + str(pinyin_data_size) + '] =\n')
    f.write('{\n')

    for eachPinyin in pinyin_info.getPinyinList():
        f.write('    ')
        for eachChar in eachPinyin:
            f.write('0x%02x, ' % ord(eachChar))
        f.write('0x00, /* ' + eachPinyin + ' */\n')

    f.write('};\n')
    f.write('\n')
       
    # encode segment
    
    f.write('const static PY_NODE_SEGMENT G_PY_NODE_SEGMENT_DAT[' + str(pinyin_info.getPinyinMaxLen() + 1) + '] =\n')
    f.write('{\n')

    nodeSegment = {}    
    for eachItem in range(0, pinyin_info.getPinyinMaxLen() + 1):
        nodeSegment[eachItem] = 0

    for eachItem in node_info.getNodeList():
        nodeSegment[len(eachItem)] += 1

    data_offset = 0
    for eachItem in nodeSegment:
        f.write('    0x%04x, ' % data_offset + '0x%04x, ' % (data_offset + nodeSegment[eachItem]) + '/* ' + str(eachItem) + ' */\n')
        data_offset += nodeSegment[eachItem]

    f.write('};\n')
    f.write('\n')

    # node item

    f.write('const static PY_NODE_ITEM G_PY_NODE_ITEM_DAT[' + str(len(node_info.getNodeList())) + '] =\n')
    f.write('{\n')

    data_offset = 0
    item_offset = 0
    
    nodeDict = node_info.getNodeDict()
    for eachNode in node_info.getNodeList():
        pinyin_total = 0
        for eachItem in nodeDict[eachNode]:
            pinyin_total += len(eachItem.pinyin_list)
        f.write('    0x%04x, ' % data_offset + '0x%04x, ' % len(eachNode) + '0x%04x, ' % item_offset + '0x%04x, ' % (item_offset + pinyin_total))
        f.write('/* ' + eachNode + ' */\n')
        data_offset += len(eachNode) + 1
        item_offset += pinyin_total

    f.write('};\n')
    f.write('\n')

    # node string

    node_data_size = data_offset

    f.write('const static PY_CHAR G_PY_NODE_STR_DAT[' + str(node_data_size) + '] =\n')
    f.write('{\n')

    nodeDict = node_info.getNodeDict()
    for eachItem in node_info.getNodeList():
        f.write('    ')
        for eachChar in eachItem:
            f.write('0x%02x, ' % ord(eachChar))
        f.write('0x00, /* ' + eachItem + ' */\n')

    f.write('};\n')
    f.write('\n')

    # node - pinyin

    node_pinyin_total = 0
    nodeDict = node_info.getNodeDict()
    for eachNode in node_info.getNodeList():
        for eachItem in nodeDict[eachNode]:
            node_pinyin_total += len(eachItem.pinyin_list)

    f.write('const static PY_NODE_PINYIN G_PY_NODE_PINYIN_DAT[' + str(node_pinyin_total) + '] =\n')
    f.write('{\n')

    for eachNode in node_info.getNodeList():
        for eachItem in nodeDict[eachNode]:
            for eachPinyin in eachItem.pinyin_list:
                f.write('    ' + '0x%08x, ' % eachItem.fuzzy_id + '0x%04x, ' % eachItem.node_type +  '0x%04x, ' % (pinyin_info.getPinyinIndex(eachPinyin)))
                f.write('/* ' + eachNode + ' '+ eachPinyin + ' */\n')

    f.write('};\n')
    f.write('\n')

    # key item

    key_code_total = 0
    keyDict = key_info.getKeyDict()
    for eachItem in key_info.getKeyList():
        key_code_total += len(keyDict[eachItem])

    f.write('const static PY_KEY_ITEM G_PY_KEY_ITEM_DAT[' + str(key_code_total) + '] =\n')
    f.write('{\n')

    for eachKey in key_info.getKeyList():
        for eachCode in keyDict[eachKey]:
            f.write('   ' + '0x%04x, ' % ord(eachKey) + '0x%04x, ' % ord(eachCode))
            f.write('/* ' + eachKey + ' '+ eachCode + ' */\n')

    f.write('};\n')
    f.write('\n')

    # han - pinyin sort by id
   
    f.write('const static PY_HAN_ITEM G_PY_HAN_ITEM_DAT[' + str(len(han_info.getHanList())) + '] =\n')
    f.write('{\n')

    for eachItem in han_info.getHanList():
        f.write('    {0x%04x, ' % eachItem.han_unicode + '0x%04x}, ' % eachItem.pinyin_id + '/* ' + eachItem.han_utf8 + ' ' + eachItem.pinyin_str + ' */\n')

    f.write('};\n')
    f.write('\n')

    # han - pinyin sort by unicode

    f.write('const static PY_HAN_R_ITEM G_PY_HAN_R_DAT[' + str(len(han_info.getHanListAscOrder())) + '] =\n')
    f.write('{\n')
    
    for eachItem in han_info.getHanListAscOrder():
        f.write('    {0x%04x, ' % eachItem.han_unicode + '0x%04x, ' % eachItem.pinyin_id + '0x%04x}, ' % han_info.getHanIdByPinyinId(eachItem.han_utf8, eachItem.pinyin_id) + '/* ' + eachItem.han_utf8 + ' ' + eachItem.pinyin_str + ' */\n')

    f.write('};\n')
    f.write('\n')

    #pinyin struct

    f.write('const static PY_HAN_DATA G_PY_HAN_DATA_DAT =\n')
    f.write('{\n')

    f.write('    0x%04x,' % len(pinyin_info.getPinyinList())    + ' /* Pinyin item total = %d */\n' % len(pinyin_info.getPinyinList()))
    f.write('    0x%04x,' % pinyin_data_size                    + ' /* Pinyin string size = %d */\n' % pinyin_data_size)
    f.write('    0x%04x,' % pinyin_info.getPinyinMaxLen()       + ' /* Pinyin max len = %d */\n' % pinyin_info.getPinyinMaxLen())
    f.write('    0x%04x,' % pinyin_info.getPinyinGroupSize()    + ' /* Hash group total = %d */\n' % pinyin_info.getPinyinGroupSize())
    f.write('    0x%04x,' % (len(han_info.getHanList()))    + ' /* Han total = %d */\n' % (len(han_info.getHanList())))
    f.write('    0x%04x,' % (pinyin_info.getPinyinMaxLen() + 1) + ' /* Node segment total = %d */\n' % (pinyin_info.getPinyinMaxLen() + 1))
    f.write('    0x%04x,' % len(node_info.getNodeList())        + ' /* Node item total = %d */\n' % len(node_info.getNodeList()))
    f.write('    0x%04x,' % node_data_size                      + ' /* Node string size = %d */\n' % node_data_size)
    f.write('    0x%04x,' % node_pinyin_total                   + ' /* Node - Pinyin total = %d */\n' % node_pinyin_total)
    f.write('    0x%04x,' % key_code_total                      + ' /* Key item total = %d */\n' % key_code_total)
    f.write('\n')
    f.write('    G_PY_PINYIN_ITEM_DAT,\n')
    f.write('    G_PY_PINYIN_STR_DAT,\n')
    f.write('    G_PY_HAN_ITEM_DAT,\n')
    f.write('    G_PY_HAN_R_DAT,\n')
    f.write('    G_PY_NODE_SEGMENT_DAT,\n')
    f.write('    G_PY_NODE_ITEM_DAT,\n')
    f.write('    G_PY_NODE_STR_DAT,\n')
    f.write('    G_PY_NODE_PINYIN_DAT,\n')
    f.write('    G_PY_KEY_ITEM_DAT,\n')
    
    f.write('};\n')
    f.write('\n')

    # han function

    f.write('const PY_HAN_DATA * PYHan_GetHanData(IM_VOID)\n')
    f.write('{\n')
    f.write('    return &G_PY_HAN_DATA_DAT;\n')
    f.write('}\n')
    f.write('\n')

    f.close()


def make_src_code(text_file_path, src_code_path):

    pinyin_info = PinyinInfo(os.path.join(text_file_path, "shengmu.txt"), os.path.join(text_file_path, "yunmu.txt"), os.path.join(text_file_path, "pinyin.txt"), os.path.join(text_file_path, "group.txt"))
    han_info = HanInfo(pinyin_info, os.path.join(text_file_path, "han.txt"))
    fuzzy_info = FuzzyInfo(pinyin_info, os.path.join(text_file_path, "fuzzy.txt"))
    encode_info = EncodeInfo(pinyin_info, fuzzy_info)
    key_info = KeyInfo(os.path.join(text_file_path, "key.txt"))
    
    print('')
    
    writeFuzzyHxxFile(os.path.join(src_code_path, "PYConst.h"), pinyin_info, fuzzy_info, encode_info, key_info, han_info)
    writePinyinHxxFile(os.path.join(src_code_path, "PYHanData.h"))
    writePinyinCxxFile(os.path.join(src_code_path, "PYHanData.c"), pinyin_info, fuzzy_info, encode_info, key_info, han_info)

    print('\nFinished!\n')


if __name__ == '__main__':

    if len(sys.argv) == 3:
        make_src_code(sys.argv[1], sys.argv[2])
    else:
        print('pinyin code generate tools')
        print('  <arg0>: text_pathname')
        print('  <arg1>: code_pathname')

        file_path = "../text"
        code_path = "../../src/pinyin"
        make_src_code(file_path, code_path)
