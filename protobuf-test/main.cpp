#include "person.pb.h"
#include <iostream>
#include <fstream>

void TestStream(const std::string& path)
{
    Person xiaoming;
    xiaoming.set_name("xiaoming");
    xiaoming.set_id(2);
    xiaoming.set_email("xiaoming@126.com");

    Person::PhoneNumber* phone = xiaoming.add_phones();
    // std::unique_ptr<Person::PhoneNumber> phone = std::make_unique<Person::PhoneNumber>();
    phone->set_number("2342342-121231");
    phone->set_type(Person::HOME);
    // xiaoming.add_phones(std::move(phone));

    {
        std::ofstream ofile(path, std::ios::out | std::ios::binary | std::ios::trunc);
        if (!xiaoming.SerializeToOstream(&ofile)) {
            std::cerr << "Failed to write msg." << std::endl;
            return;
        }
    }

    Person xiaoming2;
    {
        // 反序列化
        std::ifstream ifile(path, std::ios::in | std::ios::binary);
        if (!ifile.is_open()) {
            std::cerr << "open msg data file failed!" << std::endl;
            return;
        }

        if (!xiaoming2.ParseFromIstream(&ifile)) {
            std::cerr << "Failed to parse msg" << std::endl;
            return;
        }
    }

    std::cout << "xiaoming2 ------------------" << std::endl;
    std::cout << "Name2: " << xiaoming2.name() << std::endl;
    std::cout << "Id2: " << xiaoming2.id() << std::endl;
    std::cout << "Email2: " << xiaoming2.email() << std::endl;

    for (int i = 0; i < xiaoming2.phones_size(); ++i) {
        const Person::PhoneNumber &phone = xiaoming2.phones(i);
        std::cout << "Phone Number2: " << phone.number() << ", Type: " << phone.type() << std::endl;
    }
}

int main()
{
    Person john;
    john.set_name("john");
    john.set_id(1);
    john.set_email("xx@126.com");

    Person::PhoneNumber* phone = john.add_phones();
    phone->set_number("34343-234234");
    phone->set_type(Person::MOBILE);

    std::string serialized;
    john.SerializeToString(&serialized);

    // 反序列化
    Person jane;
    if (jane.ParseFromString(serialized)) {
        std::cout << "Name: " << jane.name() << std::endl;
        std::cout << "Id: " << jane.id() << std::endl;
        std::cout << "Email: " << jane.email() << std::endl;

        for (int i = 0; i < jane.phones_size(); ++i) {
            const Person::PhoneNumber& phone = jane.phones(i);
            std::cout << "Phone Number: " << phone.number() << ", Type: " << phone.type() << std::endl;
        }
    } else {
        std::cerr << "Failed to parse the serialized data." << std::endl;
    }

    TestStream("./xiaoming.data");

    return 0;
}