#include <iostream>
#include <fstream>
#include <cstring>
#include <stdlib.h>
#include <vector>
using namespace std;

class Staff{
    protected://这样派生类也能继承
    string no; //编号
    string name; //姓名
    int age; //年龄

    public:
    Staff(string no, string name, int age){
        this->no = no;
        this->name = name;
        this->age = age;
    }
    virtual ~Staff(){}
    //声明为public, 对外接口
    virtual void showData() const = 0; //输出员工信息(纯虚函数-派生类必须实现)
    virtual string getTitle() const = 0;//得到员工的职称(纯虚函数)
    string getNo() const{ return no;} //得到员工的编号(常函数)
    void setNo(string no){this->no = no;};//修改员工编号
    string getName() const{return name;}//得到员工的姓名
    void setName(string name){this->name = name;};//修改员工姓名
    int getAge() const{return age;}//得到员工的年龄
    void setAge(int age){this->age = age;};//修改员工年龄
    virtual double getSales()const{}//得到员工销售额(如果有的话)(虚函数-派生类可不实现)
    virtual void setSales(double sales){}//修改员工销售额(如果有的话) 
};

class Salesman: virtual public Staff{
    protected:
    double sales;//销售额

    public:
    Salesman(string no, string name, int age, double sales): Staff(no, name, age){
        this->no = no;
        this->name = name;
        this->age = age;
        this->sales = sales;
    }
    virtual ~Salesman(){}

    virtual void showData() const{ //纯虚函数重写格式需严格 
        cout<<"编号:"<<no<<" ";
        cout<<"姓名:"<<name<<" ";
        cout<<"年龄:"<<age<<" ";
        cout<<"职称:"<<"Salesman"<<"     ";
        cout<<"销售额:"<<sales<<endl;
    }

    virtual string getTitle() const{//重写,返回各个类职称
        return "Salesman";
    }

    double getSales() const{return this->sales;}
    void setSales(double sales){this->sales = sales;};//修改员工Sales
};


class Manager: virtual public Staff{
    //数据成员都由基类继承来
    public:
    Manager(string no, string name, int age): Staff(no, name, age){
        this->no = no;
        this->name = name;
        this->age = age;
    }
    virtual ~Manager(){}

    virtual void showData() const{
        cout<<"编号:"<<no<<" ";
        cout<<"姓名:"<<name<<" ";
        cout<<"年龄:"<<age<<" ";
        cout<<"职称:"<<"Manager"<<endl;
    }
    virtual string getTitle() const{
        return "Manager";
    }
    //虽然继承了getSales()和setSales()但函数体为空
};

class SalesManager: public Salesman, public Manager{
    //sales由Salesman继承来
    public:
    SalesManager(string no, string name, int age, double sales): Staff(no, name, age),
    Salesman(no, name, age, sales), Manager(no, name, age){
        this->no = no;
        this->name = name;
        this->age = age;
        this->sales = sales;
    }
    virtual ~SalesManager(){}

    virtual void showData() const{
        cout<<"编号:"<<no<<" ";
        cout<<"姓名:"<<name<<" ";
        cout<<"年龄:"<<age<<" ";
        cout<<"职称:"<<"SalesManager"<<" ";
        cout<<"销售额:"<<sales<<endl;
    }
    virtual string getTitle() const{
        return "SalesManager";
    }
    double getSales() const{return this->sales;}//防止歧义, 重写
    void setSales(double sales){this->sales = sales;};
};

struct Node{ //vector中的元素
    Staff *staff;//指向Staff的指针
    bool isEmployed;//待解雇用
};

class ManagementSystem{
    private:
    vector<Node> vec;//动态数组
    int StaffNum;//员工总人数
    //private,将功能封装, 用户只能通过SelectFunction()接口调用
    void fileIn();//文件录入
    void showStaff();//显示所有信息
    void showCount();//显示所有员工人数
    void searchStaff();//显示单个人的信息
    void addData();//添加员工
    void markStaffToBeFired();//标记待解雇员工
    void updateStaff();//修改员工信息
    void fireStaffs();//解雇isEmployed为false的员工
    void recombineFile();//保存文件并退出

    //辅助函数
    void hire(string no, string name, int age, string title, double sales = 0);//往vector中新加结点
    bool hasRepeatted(string no);//员工号为码,判断vector中是否已存在相同员工号
    bool inTitles(string title){//判断输出title是否正确
        return (!title.compare("Salesman"))||(!title.compare("Manager"))||(!title.compare("SalesManager"));
    }
    bool hasSales(string title){//判断是否拥有sales这个数据成员
        return ((!title.compare("Salesman")) || (!title.compare("SalesManager")));}
    bool isSalesman(string title){return (!title.compare("Salesman"));}//判断是否是Manager
    bool isManager(string title){return (!title.compare("Manager"));}
    bool isSalesManager(string title){return (!title.compare("SalesManager"));}
    int getStaffNum(){return StaffNum;}

    public:
    ManagementSystem();
    ~ManagementSystem(){}
    void SelectFunction();//选择功能
};

ManagementSystem::ManagementSystem(){
    this->StaffNum = 0;
    fileIn();//读接口
}

void ManagementSystem::fileIn(){
    string no, name, title;
    int age;
    double sales;

    ifstream infile("staff.txt", ios::in);
    if(!infile) cout<<"文件新创建, 请先添加员工再操作!\n"<<endl;
    infile.seekg(0, ios::end); //将文件指针指向文件末端 
	streampos fp = infile.tellg(); //fp为文件指针的偏移量 
	if (int(fp) == 0) cout<<"文件为空, 请先添加员工再操作!\n"<<endl;
	infile.seekg(0, ios::beg); //将文件指针指向文件末端
    while(infile >> no >> name >> age >> title){ //依次录入数据
        sales = 0;
        if (hasSales(title)) infile >> sales;
        hire(no, name, age, title, sales);
    }
}

//往链表末尾添加一个新结点, 同时StaffNum++;
void ManagementSystem::hire(string no, string name, int age, string title, double sales){
    Node *newNode = (Node *) malloc(sizeof(Node)); //分配一块新结点的空间, 并让 newNode指向它
    //根据不同的职位,生成不同的Node
    if (isSalesman(title)) {
        newNode->staff = new Salesman(no, name, age, sales);;
        newNode->isEmployed = true;
    } else if (isManager(title)) {
        newNode->staff = new Manager(no, name, age);
        newNode->isEmployed = true;
    } else if (isSalesManager(title)) {
        newNode->staff = new SalesManager(no, name, age, sales);
        newNode->isEmployed = true;
    }
    else{
        cout<<"输入错误, 请重新输入:"<<endl;
    }
    vec.push_back(*newNode);
    StaffNum++;//员工数要记得加1
}

//输出所有员工的信息
void ManagementSystem::showStaff(){
    for(Node it: vec){
            cout << "isEmployed:" << it.isEmployed <<" ";
            it.staff->showData(); 
    }
}

//判断员工号是否重复
bool ManagementSystem::hasRepeatted(string no){
    vector<Node>::iterator it;
        for(it = vec.begin(); it != vec.end(); it++){
            if(!no.compare(it->staff->getNo())){
                return true;
            }     
        }
}

//按员工号或者姓名索引信息
void ManagementSystem::searchStaff(){
	string searchNo, searchName;
    while(1){
    int c;
    cout<<"选择关键词(按0退出):(1)根据员工号(2)根据姓名"<<endl;
    cin>>c;
    if (cin.good() == 1);//判断是否输入的是整数
    else{cout<<"输入错误."<<endl;cin.clear();cin.sync();continue;}
    if(c == 1){
        cout<<"请输入员工号:";
        cin>>searchNo;
        for(Node it: vec){
           if(!searchNo.compare(it.staff->getNo())){
                cout << "isEmployed:" << it.isEmployed <<" ";
                it.staff->showData();
                return;
            }      
        }
        cin.ignore(100, '\n');//防止输入多余信息
        cout<<"查无此人."<<endl;        
        break;
    }
    else if(c == 2){
    	bool flag = false; 
        cout<<"请输入员工姓名:";//需考虑可能重名的情况
        cin>>searchName;
        for(Node it: vec){
           if(!searchName.compare(it.staff->getName())){
                cout << "isEmployed:" << it.isEmployed <<" ";
                it.staff->showData();
                flag = true;
            }   
        }
        if(flag) return;
        cin.ignore(100, '\n');
        cout<<"查无此人."<<endl;
        break;
    }
    else if(c == 0) return;
    else{
        cout<<"输入错误, 请重新输入"<<endl;
    }
    }
}

//输出人数
void ManagementSystem::showCount(){
    int SalesmanNum=0, ManagerNum=0, SalesmanagerNum=0;
    vector<Node>::iterator it;
    for(it = vec.begin(); it != vec.end(); it++){
        string ss = it->staff->getTitle(); 
        if(isSalesman(ss)) SalesmanNum++;
        else if(isManager(ss)) ManagerNum++;
        else if(isSalesManager(ss)) SalesmanagerNum++;
        else{}
    }
    cout<<"总人数:"<<getStaffNum()<<endl<<"Salesman人数:"<<SalesmanNum
        <<endl<<"Manager人数:"<<ManagerNum<<endl<<"Salesmanager人数:"<<SalesmanagerNum;
}

void ManagementSystem::addData(){//需考虑员工号重复情况
    int age;
    string no, name, title;
    double sales;
    while(1){
    cout<<"请输入员工号，姓名，年龄，职位，(营销额):(如果想退出请输入'#'"<<endl;
    cin>>no; //判断输入是否为#
    if(!no.compare("#")) return;
    cin>>name;
    cin>>age;//age 需为int型
    if (cin.good() == 1){cin.ignore(100, ' ');cin>>title;}//如果是double型则只取小数点前的 
    else{cout<<"年龄需为整数."<<endl;cin.clear();cin.sync();continue;}
    //cin.ignore(100, ' ');//如果是double型则只取小数点前的 
    if(inTitles(title));// title只能为3种
    else{cout<<"职位输入错误."<<endl;cin.ignore(100, '\n');continue;}
    if(hasRepeatted(no)){cout<<"员工编号重复."<<endl;continue;}
    if (hasSales(title)){
    	while(1){
    		cin >> sales;cin.ignore(100, '\n');//防止用户多输入数据
			if (cin.good() == 1);
    		else{cout<<"销售额需为复数."<<endl;cin.clear();cin.sync();continue;}
    		break; 
		}
	}
    else{
    	if(cin.eof()){//防止用户多余输入 
    		cout<<"(该员工无需输入销售额."<<endl; 
    		cin.ignore(100, '\n');	
		}
	} 
	
    hire(no, name, age, title, sales);//录入并退出函数
    break;
    }
    cout << "员工信息已录入" << endl;
}

void ManagementSystem::markStaffToBeFired(){
	string searchNo, searchName;
    while(1){
    cout<<"选择关键词(按0退出):(1)根据员工号(2)根据姓名"<<endl;
    int c;cin>>c;
    if (cin.good() == 1);
    else{cout<<"输入错误."<<endl;cin.clear();cin.sync();continue;}
    if(c == 1){
        cout<<"请输入员工号:";
        cin>>searchNo;
        for(Node &it: vec){//此处需用引用
           if(!searchNo.compare(it.staff->getNo())){
                it.isEmployed = false;
                cout<<"已标记"<<endl; 
                return;
            }      
        }
        cin.ignore(100, '\n');
        cout<<"查无此人"<<endl;
        break;
    }
    else if(c == 2){
        cout<<"请输入员工姓名:";//需考虑可能重名的情况
        cin>>searchName;
        for(Node &it: vec){
           if(!searchName.compare(it.staff->getName())){
                it.isEmployed = false;
                cout<<"已标记"<<endl; 
                return; 
            }     
        }
        cin.ignore(100, '\n');
        cout<<"查无此人"<<endl;
        break;
    }
    else if(c == 0){return;}
    else{
        cout<<"输入错误, 请重新输入"<<endl;
    }
    }
}

void ManagementSystem::updateStaff(){
    int c, age;
    string searchNo, no, name, title;
    double sales;
    cout<<"请输入员工号:";
    cin>>searchNo;

    vector<Node>::iterator it;
    for(it = vec.begin(); it != vec.end();){
        if(!searchNo.compare(it->staff->getNo())){
            no = it->staff->getNo();
            name = it->staff->getName();
            age = it->staff->getAge();
            while(1){           
            	while(1){
                	cout<<"1.员工号2.姓名3.年龄4.职位(5.营销额)\n要修改(退出请输入0):";
                	cin>>c;
                	if (cin.good() == 1);//输入需为整数
                	else{cout<<"输入错误."<<endl;cin.clear();cin.sync();continue;}
                	break;
            	}
            
            if(c == 0) return;//是0则退出 
            switch (c)
            {
            case 1:
                cout<<"请输入员工号:";cin>>no;
                cin.ignore(100, '\n');
                it->staff->setNo(no);
                break;
            case 2:
                cout<<"请输入员工姓名:";cin>>name;
                cin.ignore(100, '\n');
                it->staff->setName(name);
                break;
            case 3:
                while(1){
                    cout<<"请输入员工年龄:";cin>>age;
                    if (cin.good() == 1) cin.ignore(100, ' ');
                    else{cout<<"年龄需为整数."<<endl;cin.clear();cin.sync();continue;}
                    break;
                }
                it->staff->setAge(age);
                break;
            case 4:
                while(1){
                cout<<"请输入员工职位:";cin>>title;
                cin.ignore(100, '\n');
                if(inTitles(title)) ;// title只能为3种
                else{cout<<"职位输入错误."<<endl;cin.ignore(100, '\n');continue;}
                break;
                }

                if(isManager(it->staff->getTitle())){
                    if(isManager(title)){cout<<"职位重复"<<endl;return;}
                    else{
                        cout<<"请再输入其销售额:";
                        cin>>sales;
                        Node *newNode = (Node *) malloc(sizeof(Node)); //分配一块新结点的空间, 并让newNode指向它
                        if (isSalesman(title)) {
                            newNode->staff = new Salesman(no, name, age, sales);
                            newNode->isEmployed = true;
                        }
                        else{
                            newNode->staff = new SalesManager(no, name, age, sales);
                            newNode->isEmployed = true;
                        }
                        
                        it = vec.erase(it); //返回指向被删除元素的下一个的指针 
                        it = vec.insert(it, *newNode);//往指向元素前增加一个元素 
                    }
                }
                else{
                    if(!title.compare(it->staff->getTitle())){
                        cout<<"职位重复"<<endl;
                        return;
                    }
                    else{
                        if(isManager(title)){
                            Node *newNode = (Node *) malloc(sizeof(Node)); 
                            newNode->staff = new Manager(no, name, age);
                            newNode->isEmployed = true;
                            it = vec.erase(it); //返回指向被删除元素的下一个的指针 
                            it = vec.insert(it, *newNode);//往指向元素前增加一个元素 
                        }
                        else if(isSalesman(title)){
                            Node *newNode = (Node *) malloc(sizeof(Node)); 
                            newNode->staff = new Salesman(no, name, age, it->staff->getSales());
                            newNode->isEmployed = true;
                            it = vec.erase(it); 
                            it = vec.insert(it, *newNode);
                        }
                        else{
                            Node *newNode = (Node *) malloc(sizeof(Node));
                            newNode->staff = new SalesManager(no, name, age, it->staff->getSales());
                            newNode->isEmployed = true;
                            it = vec.erase(it); 
                            it = vec.insert(it, *newNode);
                        }
                    }

                }
                break;
            case 5:
                if(isManager(it->staff->getTitle())){
                    cout<<"经理无销售额一说."<<endl;
                    return;
                }
                else{
                    cout<<"请输入销售额:";cin>>sales;
                    cin.ignore(100, '\n');
                    it->staff->setSales(sales);
                }
                break;
            
            default:
                break;
            }
			cout << "员工信息已修改." << endl;
            return;
            }      
        }
        else it++; 
    
    }
    cout<<"查无此人"<<endl;
}

void ManagementSystem::fireStaffs(){
    vector<Node>::iterator it;
    for(it = vec.begin(); it != vec.end();){
        if(it->isEmployed == false){
            it = vec.erase(it); //返回指向被删除元素的下一个的指针
        }//如果删除的是最后一个元素则指向删除后的最后一个 
        else it++;
    }
    cout<<"完成操作"<<endl;
}

void ManagementSystem::recombineFile(){
    fireStaffs();//保存前删除待解雇员工
    ofstream outfile("staff.txt", ios::out | ios::trunc);//重写文件
    vector<Node>::iterator it;
    for(it = vec.begin(); it != vec.end(); it++){
        outfile<<it->staff->getNo()<<" "<<it->staff->getName()
		<<" "<<it->staff->getAge()<<" "<<it->staff->getTitle()<<" ";
        if(!isManager(it->staff->getTitle()))
	    	outfile<<it->staff->getSales()<<endl;
        else outfile<<endl; 
    }
    outfile.close();
}

void ManagementSystem::SelectFunction(){
    int c, age;
    string no, name, title;
    double sales;
    cout<<"欢迎使用销售公司员工管理系统!"<<endl;
    while(true){
    	cout<<
		" -------------------------------------------------\n";
        cout<<
		"    选择功能:(1)查询所有员工信息 (2)查询员工信息\n"<<
		" (3)查询人数 (4)标记待解雇员工   (5)解雇已标记员工\n"<<
		" (6)添加员工 (7)修改员工信息     (0)保存并退出    \n"<<
		" -------------------------------------------------"<<endl;
		cout<<"选择:"; 
        cin>>c;
        if (cin.good() == 1);
        else{cout<<"输入错误, 请重新输入."<<endl;cin.clear();cin.sync();continue;}
        switch (c)
        {
        case 1:
            showStaff();cout<<endl;
            break;
        case 2:
            searchStaff();cout<<endl;
            break;
        case 3:
            showCount();cout<<endl;
            break;
        case 4:
            markStaffToBeFired();
            break;
        case 5:
            fireStaffs();cout<<endl;
            break;
        case 6:
            addData();cout<<endl;
            break;
        case 7:
            updateStaff();cout<<endl;
            break;
        case 0:
            recombineFile();cout<<endl;
            break;
        default:
            cout<<"输入错误, 请重新输入."<<endl;
            cin.ignore(100, '\n');//防止错误输入 清除缓冲区 
            break;
        }
        if(c == 0) break;
    }
}

int main(){
    ManagementSystem ms;
    ms.SelectFunction();
    return 0;
}
