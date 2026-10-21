#include <iostream>
#include <string>
int main(){
    std::string name;
    std::cout<<"请输入机器人名称："<<std::endl;       
    std::getline(std::cin,name);
    if (name.empty()) {
    std::cout << "名称不能为空。" << std::endl;
    return 0;
}
    if (name.size() >12){
        std::cout<<"名称过长。"<<std::endl;
        return 0;
    }
    std::string message = "你好，" + name + "!";
    std::cout<< message <<std::endl;
    std::cout << "名称长度：" << name.size() << std::endl;
    char first = name[0];
    std::cout << "首字母：" << first << std::endl;
    char last = name[name.size() -1];
    std::cout << "末字母：" << last << std::endl;
    name[0]='S';
    std::cout << "修改后：" << name << std::endl;
    std::cout << "原欢迎语：" << message << std::endl;
    message = "你好，" + name + "!";
    std::cout << "新欢迎语：" << message << std::endl;
    return 0;
}