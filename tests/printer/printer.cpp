/*
Copyright libCellML Contributors

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "gtest/gtest.h"

#include <libcellml>

TEST(Printer, printEmptyModel)
{
    const std::string e =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<model xmlns=\"http://www.cellml.org/cellml/2.0#\"/>";
    libcellml::ModelPtr m = std::make_shared<libcellml::Model>();

    libcellml::Printer p;

    const std::string a = p.printModel(m);

    EXPECT_EQ(e, a);
}

TEST(Printer, printEmptyModelAllocatePointer)
{
    const std::string e =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<model xmlns=\"http://www.cellml.org/cellml/2.0#\"/>";
    auto m = new libcellml::Model();

    libcellml::Printer printer;
    const std::string a = printer.printModel(m);

    EXPECT_EQ(e, a);
    delete m;
}

TEST(Printer, printEmptyUnits)
{
    const std::string e;
    libcellml::Units u;

    libcellml::Printer printer;
    const std::string a = printer.printUnits(u);
    EXPECT_EQ(e, a);
}

TEST(Printer, printEmptyVariable)
{
    const std::string e = "<variable/>";
    libcellml::Variable v;

    libcellml::Printer printer;
    const std::string a = printer.printVariable(v);
    EXPECT_EQ(e, a);
}

TEST(Printer, printEmptyComponent)
{
    const std::string e = "<component/>";
    libcellml::Component c;

    libcellml::Printer printer;
    const std::string a = printer.printComponent(c);
    EXPECT_EQ(e, a);
}

TEST(Printer, printEmptyReset)
{
    const std::string e = "<reset/>";
    libcellml::Reset r;

    libcellml::Printer printer;
    const std::string a = printer.printReset(r);
    EXPECT_EQ(e, a);
}

TEST(Printer, printEncapsulation)
{
    const std::string e_parent =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<model xmlns=\"http://www.cellml.org/cellml/2.0#\">"
        "<component/>"
        "<component/>"
        "<encapsulation>"
        "<component_ref>"
        "<component_ref/>"
        "</component_ref>"
        "</encapsulation>"
        "</model>";
    const std::string e_child = "<component/>";

    libcellml::Model model;
    libcellml::ComponentPtr parent = std::make_shared<libcellml::Component>();
    libcellml::ComponentPtr child = std::make_shared<libcellml::Component>();
    parent->addComponent(child);

    model.addComponent(parent);

    libcellml::Printer printer;
    std::string a_parent = printer.printModel(model);
    EXPECT_EQ(e_parent, a_parent);
    std::string a_child = printer.printComponent(child);
    EXPECT_EQ(e_child, a_child);
}

TEST(Printer, printEncapsulationWithNames)
{
    const std::string e_parent =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<model xmlns=\"http://www.cellml.org/cellml/2.0#\">"
        "<component name=\"parent_component\"/>"
        "<component name=\"child_component\"/>"
        "<encapsulation>"
        "<component_ref component=\"parent_component\">"
        "<component_ref component=\"child_component\"/>"
        "</component_ref>"
        "</encapsulation>"
        "</model>";
    const std::string e_child = "<component name=\"child_component\"/>";

    libcellml::Model model;
    libcellml::ComponentPtr parent = std::make_shared<libcellml::Component>();
    parent->setName("parent_component");
    libcellml::ComponentPtr child = std::make_shared<libcellml::Component>();
    child->setName("child_component");
    parent->addComponent(child);

    model.addComponent(parent);

    libcellml::Printer printer;
    std::string a_parent = printer.printModel(model);
    EXPECT_EQ(e_parent, a_parent);
    std::string a_child = printer.printComponent(child);
    EXPECT_EQ(e_child, a_child);
}
