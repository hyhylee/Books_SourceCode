#include <iostream>
#include <memory>
#include <vector>

class Widget : public std::enable_shared_from_this<Widget> 
{
    public:
    void process();
};

std::vector<std::shared_ptr<Widget>> processedWidgets;

#define A 2

void Widget::process()
{
    //...                                     //处理Widget

#if A == 1
    //本语句和Widget* p = new Widget(); p->process()配合没有问题, vector中的std::shared_ptr会自动为this建立一个控制块cb,但暴露裸指针p是十分有害的。强烈不推荐!
    processedWidgets.emplace_back(this);    //把它加到处理过的Widget的Widget的列表中去，如果main中使用了std::shared_ptr，则使用this是错误的！
#elif A==2
    //本语句和std::shared_ptr<Widget> p(new Widget); p->process();配合没有问题.
    processedWidgets.emplace_back(shared_from_this());    
#endif
}
//void makeLogEntry(auto* pw){}
void makeLogEntry(auto pw)
{
;
}


int main(void)
{
    auto loggingDel = [] (Widget *pw)       //自定义deleter
    {
        makeLogEntry(pw);                     
        delete pw;                  
    };

    std::unique_ptr<Widget, decltype(loggingDel)>//deleter的类型是指针类型的一部分  
        upw(new Widget, loggingDel);  //使用裸指针会创建控制块cb, shared_ptr和cb一一对应，为了避免创建多余的cb，坚决不要暴露裸指针。

    std::shared_ptr<Widget>                 
        spw(new Widget, loggingDel);//deleter的类型不是指针类型的一部分


    //Widget w; w.process();//如果w需要进行资源管理(栈对象不能解决动态问题)，千万别这么用.main返回时w栈对象首先被析构，导致智能指针管理失效，智能指针推出的时候析构资源失败. 
#if A==1
    Widget* p = new Widget();
    p->process();
#elif A ==2
    std::shared_ptr<Widget> p(new Widget);//使用shared_ptr，因此process()中必须使用shared_from_this()函数返回this的智能指针对象。
    p->process();
#endif

    return 0;

}



