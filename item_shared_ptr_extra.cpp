#include <iostream>
#include <memory> // for std::shared_ptr
#include <string>

using namespace std; 
class Person
{
public:
	std::string m_name;
	std::shared_ptr<Person> m_partner; // initially created empty
 
public:
		
	Person(const std::string &name): m_name(name)
	{ 
		std::cout << m_name << " created\n";
	}
	~Person()
	{
		std::cout << m_name << " destroyed\n";
	}
 
	friend bool partnerUp(std::shared_ptr<Person> &p1, std::shared_ptr<Person> &p2)
	{
		if (!p1 || !p2)
			return false;
 
		p1->m_partner = p2;
		p2->m_partner = p1;
 
		std::cout << p1->m_name << " is now partnered with " << p2->m_name << "\n";
 
		return true;
	}
};
 
class Person1
{
public:
	std::string m_name;
	std::weak_ptr<Person1> m_partner; // initially created empty
 
public:
		
	Person1(const std::string &name): m_name(name)
	{ 
		std::cout << m_name << " created\n";
	}
	~Person1()
	{
		std::cout << m_name << " destroyed\n";
        cout << "m_partner<Weak_pointer>'s use_count: " << m_partner.use_count() << endl;
	}
 
	friend bool partnerUp(std::shared_ptr<Person1> &p1, std::shared_ptr<Person1> &p2)
	{
		if (!p1 || !p2)
			return false;
 
		p1->m_partner = p2;
		p2->m_partner = p1;
 
		std::cout << p1->m_name << " is now partnered with " << p2->m_name << "\n";
        cout << p1->m_name << "'s m_partner(weak_point)'s use_count()" << p1->m_partner.use_count();
        cout << p2->m_name << "'s m_partner(weak_point)'s use_count()" << p2->m_partner.use_count();        
		return true;
	}
};

int main()
{
	auto lucy = std::make_shared<Person>("Lucy"); // create a Person named "Lucy"
	auto ricky = std::make_shared<Person>("Ricky"); // create a Person named "Ricky"

	partnerUp(lucy, ricky); // Make "Lucy" point to "Ricky" and vice-versa
    cout << "lucy's use_count : " << lucy.use_count() << endl;
    cout << "ricky's use_count : " << ricky.use_count() << endl;

    /* Use reset() of shared_ptr, good */
    lucy->m_partner.reset();
    cout << "After lucy's reset, ricky's use_cout(): " << ricky.use_count() << endl;

    /* Refine Person's partner pointer type */
    auto lucy1 = std::make_shared<Person1>("Lucy");
    auto ricky1 = std::make_shared<Person1>("Ricky");
    /* New partner function */
    partnerUp(lucy1, ricky1);
    cout << "After partnerUping : lucy1's use_count : " << lucy1.use_count() << endl;
    //ricky1->m_partner(lucy1);

    /* Get weak_pointer's use_count */
    cout << "Weak_pointer's use_count, lucy1's partner(weak_pointer) : " << lucy1->m_partner.use_count() << endl;
    //cout << "Shared"

	return 0;
}