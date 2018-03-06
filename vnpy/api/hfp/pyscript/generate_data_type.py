# encoding: UTF-8

__author__ = ''
import re

# C++和python类型的映射字典
type_dict = {
    'int': 'int',
    'char': 'char',
    'double': 'float',
    'short': 'int',
    'string': 'string'
}

def main():
    """主函数"""
    try:
        fcpp = open('HFPTradingClient.h','r')
        fpy = open('hfp_data_type.py', 'w')

        fpy.write('# encoding: UTF-8\n')
        fpy.write('\n')
        fpy.write('defineDict = {}\n')
        fpy.write('typedefDict = {}\n')
        fpy.write('\n')

        recording = True
        for line in fcpp:
            if "typedef enum" in line:
                recording = True
            if "}" in line:
                recording = False
            if  recording and "=" in line:
                if "//" not in line:
                    line = line + "//"
                output_line = re.sub(",| |\n|\t|'", "", line)
                output_line = re.sub("^", "typedefDict['",output_line)
                output_line = re.sub("=", "'] = '",output_line)
                output_line = re.sub("//", "' ## ",output_line)
                fpy.write(output_line + "\n")

        fcpp.close()
        fpy.close()

        print u'data_type.py生成过程完成'
    except:
        print u'data_type.py生成过程出错'


if __name__ == '__main__':
    main()

