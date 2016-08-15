/*
Copyright 2015 University of Auckland

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

#include "libcellml/parser.h"

#include <string>
#include <vector>

#include "libcellml/component.h"
#include "libcellml/error.h"
#include "libcellml/import.h"
#include "libcellml/model.h"
#include "libcellml/variable.h"
#include "xmldoc.h"

namespace libcellml {

/**
 * @brief The Parser::ParserImpl struct.
 *
 * The private implementation for the Parser class.
 */
struct Parser::ParserImpl
{
public:
    Format mFormat;
    Parser *mParser;

    /**
     * @brief Update the @p model with attributes parsed from a @c std::string.
     *
     * Update the @p model with attributes and entities parsed from
     * the @c std::string @p input. Any entities or attributes in @p model with names
     * matching those in @p input will be overwritten.
     *
     * @param model The @c ModelPtr to update.
     * @param input The string to parse and update the @p model with.
     */
    void loadModel(const ModelPtr &model, const std::string &input);

private:
    /**
     * @brief Update the @p component with attributes parsed from @p node.
     *
     * Update the @p component with attributes and entities parsed from
     * the XML @p node. Any entities or attributes in @p component with names
     * matching those in @p node will be overwritten.
     *
     * @param component The @c ComponentPtr to update.
     * @param node The @c XmlNodePtr to parse and update the @p component with.
     */
    void loadComponent(const ComponentPtr &component, const XmlNodePtr &node);

    /**
     * @brief Update the @p model with a connection parsed from @p node.
     *
     * Update the @p model with connection information parsed from
     * the XML @p node. Connection information from @p node will be added
     * to any variable equivalence relationships already existing in @p model.
     *
     * @param model The @c ModelPtr to update.
     * @param node The @c XmlNodePtr to parse and update the model with.
     */
    void loadConnection(const ModelPtr &model, const XmlNodePtr &node);

    /**
     * @brief Update the @p model with an encapsulation parsed from @p node.
     *
     * Update the @p model with encapsulation information parsed from
     * the XML @p node. Encapsulation relationships from @p node will be added
     * to any encapsulations relationships already in @p model.
     *
     * @param model The @c ModelPtr to update.
     * @param node The @c XmlNodePtr to parse and update the model with.
     */
    void loadEncapsulation(const ModelPtr &model, const XmlNodePtr &node);

    /**
     * @brief Update the @p import with attributes parsed from @p node and add any imported
     * components or units it to the @p model.
     *
     * Update the @p import with attributes parsed from @p node and add any imported
     * components or units to the @p model. If any attributes exist in @p import
     * with names matching those in @p node, they will be overwritten. Likewise,
     * any imported components or units of the same name already in @p model will
     * be overwritten by those parsed from @p node.
     *
     * @param import The @c ImportPtr to update.
     * @param model The @c ModelPtr to add imported components/units to.
     * @param node The @c XmlNodePtr to parse and update the @p import with.
     */
    void loadImport(const ImportPtr &import, const ModelPtr &model, const XmlNodePtr &node);

    /**
     * @brief Update the @p units with attributes parsed from @p node.
     *
     * Update the @p units by parsing the XML @p node.
     * Existing attributes in @p units with names
     * matching those in @p node will be overwritten.
     *
     * @param units The @c UnitsPtr to update.
     * @param node The @c XmlNodePtr to parse and update the @p units with.
     */
    void loadUnits(const UnitsPtr &units, const XmlNodePtr &node);

    /**
     * @brief Update the @p units with a unit parsed from @p node.
     *
     * Update the @p units with a unit parsed from the XML @p node.
     * If a unit with the same name exists in @p units, it will be
     * overwritten by the unit from @p node.
     *
     * @param units The @c UnitsPtr to update.
     * @param node The unit @c XmlNodePtr to parse and update the @p units with.
     */
    void loadUnit(const UnitsPtr &units, const XmlNodePtr &node);

    /**
     * @brief Update the @p variable with attributes parsed from @p node.
     *
     * Update the @p variable with attributes parsed from
     * the XML @p node. Existing attributes in @p variable with names
     * matching those in @p node will be overwritten.
     *
     * @param variable The @c VariablePtr to update.
     * @param node The @c XmlNodePtr to parse and update the @p variable with.
     */
    void loadVariable(const VariablePtr &variable, const XmlNodePtr &node);

    /**
     * @brief Try to convert and return the unit @p attribute value to a @c double.
     *
     * Try to convert the unit @p attribute value to a @c double. Return the new
     * @c double value if possible. Otherwise, flag an error and return the @p defaultValue.
     *
     * @param defaultValue The default value for the unit attribute.
     * @param attribute The @c XmlAttributePtr with the value to convert.
     * @param node The @c XmlNodePtr the @p attribute is within.
     * @param units The units the unit @p attribute is relevant to.
     *
     * @return The unit @p attribute value if it can be converted to a @c double;
     * the @p defaultValue otherwise.
     *
     */
    double convertUnitAttributeValueToDouble(double &defaultValue, const XmlAttributePtr &attribute,
                                             const XmlNodePtr &node, const UnitsPtr &units);

    /**
     * @brief Check if the @p input @c std::string has any non-whitespace characters.
     *
     * If the @p input @c std::string has any non-whitespace characters, return
     * @c true, otherwise return @c false.
     *
     * @param input The string to check for non-whitespace characters.
     *
     * @return @c true if @p input contains non-whitespace characters and @c false otherwise.
     */
    bool isNotWhitespace(std::string &input);
};

Parser::Parser(Format format)
    : mPimpl(new ParserImpl())
{
    mPimpl->mFormat = format;
    mPimpl->mParser = this;
}

Parser::~Parser()
{
    delete mPimpl;
}

Parser::Parser(const Parser& rhs)
    : Logger(rhs)
    , mPimpl(new ParserImpl())
{
    mPimpl->mFormat = rhs.mPimpl->mFormat;
    mPimpl->mParser = rhs.mPimpl->mParser;
}

Parser::Parser(Parser &&rhs)
    : Logger(std::move(rhs))
    , mPimpl(rhs.mPimpl)
{
    rhs.mPimpl = nullptr;
}

Parser& Parser::operator=(Parser p)
{
    Logger::operator =(p);
    p.swap(*this);
    return *this;
}

void Parser::swap(Parser &rhs)
{
    std::swap(this->mPimpl, rhs.mPimpl);
}

ModelPtr Parser::parseModel(const std::string &input)
{
    ModelPtr model = std::make_shared<Model>();
    updateModel(model, input);
    return model;
}

void Parser::updateModel(const ModelPtr &model, const std::string &input)
{
    if (mPimpl->mFormat == Format::XML) {
        mPimpl->loadModel(model, input);
    }
}

void Parser::ParserImpl::loadModel(const ModelPtr &model, const std::string &input)
{
    XmlDocPtr doc = std::make_shared<XmlDoc>();
    doc->parse(input);
    // Copy any XML parsing errors into the common parser error handler.
    if (doc->xmlErrorCount() > 0) {
        for (size_t i = 0; i < doc->xmlErrorCount(); ++i) {
            ErrorPtr err = std::make_shared<Error>();
            err->setDescription(doc->getXmlError(i));
            err->setKind(Error::Kind::XML);
            mParser->addError(err);
        }
    }
    const XmlNodePtr node = doc->getRootNode();
    if (!node) {
        ErrorPtr err = std::make_shared<Error>();
        err->setDescription("Could not get a valid XML root node from the provided input.");
        err->setKind(Error::Kind::XML);
        mParser->addError(err);
        return;
    } else if (!node->isType("model")) {
        ErrorPtr err = std::make_shared<Error>();
        err->setDescription("Model root node is of invalid type '" + node->getType() +
                            "'. A valid CellML root node should be of type 'model'.");
        err->setModel(model);
        err->setKind(Error::Kind::MODEL);
        mParser->addError(err);
        return;
    }
    // Get model attributes.
    XmlAttributePtr attribute = node->getFirstAttribute();
    while (attribute) {
        if (attribute->isType("name")) {
            model->setName(attribute->getValue());
        } else if (attribute->isType("id")) {
            model->setId(attribute->getValue());
        } else {
            ErrorPtr err = std::make_shared<Error>();
            err->setDescription("Model '" + node->getAttribute("name") +
                                "' has an invalid attribute '" + attribute->getType() + "'.");
            err->setModel(model);
            err->setKind(Error::Kind::MODEL);
            mParser->addError(err);
        }
        attribute = attribute->getNext();
    }
    // Get model children (CellML entities).
    XmlNodePtr childNode = node->getFirstChild();
    while (childNode) {
        if (childNode->isType("component")) {
            ComponentPtr component = std::make_shared<Component>();
            loadComponent(component, childNode);
            model->addComponent(component);
        } else if (childNode->isType("units")) {
            UnitsPtr units = std::make_shared<Units>();
            loadUnits(units, childNode);
            model->addUnits(units);
        } else if (childNode->isType("import")) {
            ImportPtr import = std::make_shared<Import>();
            loadImport(import, model, childNode);
        } else if (childNode->isType("encapsulation")) {
            // An encapsulation should not have attributes.
            if (childNode->getFirstAttribute()) {
                XmlAttributePtr attribute = childNode->getFirstAttribute();
                while (attribute) {
                    ErrorPtr err = std::make_shared<Error>();
                    err->setDescription("Encapsulation in model '" + model->getName() +
                                        "' has an invalid attribute '" + attribute->getType() + "'.");
                    err->setModel(model);
                    err->setKind(Error::Kind::ENCAPSULATION);
                    mParser->addError(err);
                    attribute = attribute->getNext();
                }
            }
            // Load encapsulated component_refs.
            XmlNodePtr componentRefNode = childNode->getFirstChild();
            if (componentRefNode) {
                // This component_ref and its child and sibling elements will be loaded
                // and error-checked in loadEncapsulation().
                loadEncapsulation(model, componentRefNode);
            } else {
                ErrorPtr err = std::make_shared<Error>();
                err->setDescription("Encapsulation in model '" + model->getName() +
                                    "' does not contain any child elements.");
                err->setModel(model);
                err->setKind(Error::Kind::ENCAPSULATION);
                mParser->addError(err);
            }
        } else if (childNode->isType("connection")) {
            loadConnection(model, childNode);
        } else if (childNode->isType("text")) {
            std::string textNode = childNode->convertToString();
            // Ignore whitespace when parsing.
            if (isNotWhitespace(textNode)) {
                ErrorPtr err = std::make_shared<Error>();
                err->setDescription("Model '" + model->getName() +
                                    "' has an invalid non-whitespace child text element '" + textNode + "'.");
                err->setModel(model);
                err->setKind(Error::Kind::MODEL);
                mParser->addError(err);
            }
        } else {
            ErrorPtr err = std::make_shared<Error>();
            err->setDescription("Model '" + model->getName() +
                                "' has an invalid child element '" + childNode->getType() + "'.");
            err->setModel(model);
            err->setKind(Error::Kind::MODEL);
            mParser->addError(err);
        }
        childNode = childNode->getNext();
    }
}

void Parser::ParserImpl::loadComponent(const ComponentPtr &component, const XmlNodePtr &node)
{
    XmlAttributePtr attribute = node->getFirstAttribute();
    while (attribute) {
        if (attribute->isType("name")) {
            component->setName(attribute->getValue());
        } else if (attribute->isType("id")) {
            component->setId(attribute->getValue());
        } else {
            ErrorPtr err = std::make_shared<Error>();
            err->setDescription("Component '" + node->getAttribute("name") +
                                "' has an invalid attribute '" + attribute->getType() + "'.");
            err->setComponent(component);
            err->setKind(Error::Kind::COMPONENT);
            mParser->addError(err);
        }
        attribute = attribute->getNext();
    }
    XmlNodePtr childNode = node->getFirstChild();
    while (childNode) {
        if (childNode->isType("variable")) {
            VariablePtr variable = std::make_shared<Variable>();
            loadVariable(variable, childNode);
            component->addVariable(variable);
        } else if (childNode->isType("units")) {
            UnitsPtr units = std::make_shared<Units>();
            loadUnits(units, childNode);
            component->addUnits(units);
        } else if (childNode->isType("math")) {
            std::string math = childNode->convertToString();
            component->setMath(math);
        } else if (childNode->isType("text")) {
            std::string textNode = childNode->convertToString();
            // Ignore whitespace when parsing.
            if (isNotWhitespace(textNode)) {
                ErrorPtr err = std::make_shared<Error>();
                err->setDescription("Component '" + component->getName() +
                                    "' has an invalid non-whitespace child text element '" + textNode + "'.");
                err->setComponent(component);
                err->setKind(Error::Kind::COMPONENT);
                mParser->addError(err);
            }
        } else {
            ErrorPtr err = std::make_shared<Error>();
            err->setDescription("Component '" + component->getName() +
                                "' has an invalid child element '" + childNode->getType() + "'.");
            err->setComponent(component);
            err->setKind(Error::Kind::COMPONENT);
            mParser->addError(err);
        }
        childNode = childNode->getNext();
    }
}

void Parser::ParserImpl::loadUnits(const UnitsPtr &units, const XmlNodePtr &node)
{
    XmlAttributePtr attribute = node->getFirstAttribute();
    while (attribute) {
        if (attribute->isType("name")) {
            units->setName(attribute->getValue());
        } else if (attribute->isType("id")) {
            units->setId(attribute->getValue());
        } else if (attribute->isType("base_unit")) {
            if (attribute->getValue() == "yes") {
                units->setBaseUnit(true);
            } else if (attribute->getValue() == "no") {
                units->setBaseUnit(false);
            } else {
                ErrorPtr err = std::make_shared<Error>();
                err->setDescription("Units '" + units->getName() +
                                    "' has an invalid base_unit attribute value '" + attribute->getValue() +
                                    "'. Valid options are 'yes' or 'no'.");
                err->setUnits(units);
                err->setKind(Error::Kind::UNITS);
                mParser->addError(err);
            }
        } else {
            ErrorPtr err = std::make_shared<Error>();
            err->setDescription("Units '" + units->getName() +
                                "' has an invalid attribute '" + attribute->getType() + "'.");
            err->setUnits(units);
            err->setKind(Error::Kind::UNITS);
            mParser->addError(err);
        }
        attribute = attribute->getNext();
    }
    XmlNodePtr childNode = node->getFirstChild();
    while (childNode) {
        if (childNode->isType("unit")) {
            loadUnit(units, childNode);
        } else if (childNode->isType("text")) {
            std::string textNode = childNode->convertToString();
            // Ignore whitespace when parsing.
            if (isNotWhitespace(textNode)) {
                ErrorPtr err = std::make_shared<Error>();
                err->setDescription("Units '" + units->getName() +
                                    "' has an invalid non-whitespace child text element '" + textNode + "'.");
                err->setUnits(units);
                err->setKind(Error::Kind::UNITS);
                mParser->addError(err);
            }
        } else {
            ErrorPtr err = std::make_shared<Error>();
            err->setDescription("Units '" + units->getName() +
                                "' has an invalid child element '" + childNode->getType() + "'.");
            err->setUnits(units);
            err->setKind(Error::Kind::UNITS);
            mParser->addError(err);
        }
        childNode = childNode->getNext();
    }
}

void Parser::ParserImpl::loadUnit(const UnitsPtr &units, const XmlNodePtr &node)
{
    std::string name = "";
    std::string prefix = "";
    double exponent = 1.0;
    double multiplier = 1.0;
    double offset = 0.0;
    // A unit should not have any children.
    if (node->getFirstChild()) {
        XmlNodePtr childNode = node->getFirstChild();
        while (childNode) {
            if (childNode->isType("text")) {
                std::string textNode = childNode->convertToString();
                // Ignore whitespace when parsing.
                if (isNotWhitespace(textNode)) {
                    ErrorPtr err = std::make_shared<Error>();
                    err->setDescription("Unit '" + node->getAttribute("units") +
                                        "' in units '" + units->getName() +
                                        "' has an invalid non-whitespace child text element '" + textNode + "'.");
                    err->setUnits(units);
                    err->setKind(Error::Kind::UNITS);
                    mParser->addError(err);
                }
            } else {
                ErrorPtr err = std::make_shared<Error>();
                err->setDescription("Unit '" + node->getAttribute("units") +
                                    "' in units '" + units->getName() +
                                    "' has an invalid child element '" + childNode->getType() + "'.");
                err->setUnits(units);
                mParser->addError(err);
                err->setKind(Error::Kind::UNITS);
            }
            childNode = childNode->getNext();
        }
    }
    // Parse the unit attributes.
    XmlAttributePtr attribute = node->getFirstAttribute();
    while (attribute) {
        if (attribute->isType("units")) {
            name = attribute->getValue();
        } else if (attribute->isType("prefix")) {
            prefix = attribute->getValue();
        } else if (attribute->isType("exponent")) {
            exponent = convertUnitAttributeValueToDouble(exponent, attribute, node, units);
        } else if (attribute->isType("multiplier")) {
            multiplier = convertUnitAttributeValueToDouble(multiplier, attribute, node, units);
        } else if (attribute->isType("offset")) {
            offset = convertUnitAttributeValueToDouble(offset, attribute, node, units);
        } else {
            ErrorPtr err = std::make_shared<Error>();
            err->setDescription("Unit '" + node->getAttribute("units") +
                                "' in units '" + units->getName() +
                                "' has an invalid attribute '" + attribute->getType() + "'.");
            err->setUnits(units);
            err->setKind(Error::Kind::UNITS);
            mParser->addError(err);
        }
        attribute = attribute->getNext();
    }
    // Add this unit to the parent units.
    units->addUnit(name, prefix, exponent, multiplier, offset);
}

double Parser::ParserImpl::convertUnitAttributeValueToDouble(double &defaultValue, const XmlAttributePtr &attribute,
                                                             const XmlNodePtr &node, const UnitsPtr &units)
{
    double value = defaultValue;
    // Try to convert the unit attribute value to double.
    try
    {
        value = std::stod(attribute->getValue());
    // On failure, flag error and use the default value.
    } catch (std::exception) {
        ErrorPtr err = std::make_shared<Error>();
        err->setDescription("Unit '" + node->getAttribute("units") +
                            "' in units '" + units->getName() +
                            "' has an attribute '" + attribute->getType() +
                            "' with a value '" + attribute->getValue() +
                            "' that cannot be converted to a decimal number.");
        err->setUnits(units);
        err->setKind(Error::Kind::UNITS);
        mParser->addError(err);
    }
    return value;
}

void Parser::ParserImpl::loadVariable(const VariablePtr &variable, const XmlNodePtr &node)
{
    // A variable should not have any children.
    if (node->getFirstChild()) {
        XmlNodePtr childNode = node->getFirstChild();
        while (childNode) {
            if (childNode->isType("text")) {
                std::string textNode = childNode->convertToString();
                // Ignore whitespace when parsing.
                if (isNotWhitespace(textNode)) {
                    ErrorPtr err = std::make_shared<Error>();
                    err->setDescription("Variable '" + node->getAttribute("name") +
                                        "' has an invalid non-whitespace child text element '" + textNode + "'.");
                    err->setVariable(variable);
                    err->setKind(Error::Kind::VARIABLE);
                    mParser->addError(err);
                }
            } else {
                ErrorPtr err = std::make_shared<Error>();
                err->setDescription("Variable '" + node->getAttribute("name") +
                                    "' has an invalid child element '" + childNode->getType() + "'.");
                err->setVariable(variable);
                err->setKind(Error::Kind::VARIABLE);
                mParser->addError(err);
            }
            childNode = childNode->getNext();
        }
    }
    XmlAttributePtr attribute = node->getFirstAttribute();
    while (attribute) {
        if (attribute->isType("name")) {
            variable->setName(attribute->getValue());
        } else if (attribute->isType("id")) {
            variable->setId(attribute->getValue());
        } else if (attribute->isType("units")) {
            variable->setUnits(attribute->getValue());
        } else if (attribute->isType("interface")) {
            variable->setInterfaceType(attribute->getValue());
        } else if (attribute->isType("initial_value")) {
            variable->setInitialValue(attribute->getValue());
        } else {
            ErrorPtr err = std::make_shared<Error>();
            err->setDescription("Variable '" + node->getAttribute("name") +
                                "' has an invalid attribute '" + attribute->getType() + "'.");
            err->setVariable(variable);
            err->setKind(Error::Kind::VARIABLE);
            mParser->addError(err);
        }
        attribute = attribute->getNext();
    }
}

void Parser::ParserImpl::loadConnection(const ModelPtr &model, const XmlNodePtr &node)
{
    // Define types for variable and component pairs.
    typedef std::pair <std::string, std::string> NamePair;
    typedef std::vector<NamePair> NamePairMap;
    typedef NamePairMap::const_iterator NameMapIterator;

    // A connection should not have attributes.
    if (node->getFirstAttribute()) {
        XmlAttributePtr attribute = node->getFirstAttribute();
        while (attribute) {
            ErrorPtr err = std::make_shared<Error>();
            err->setDescription("Connection in model '" + model->getName() +
                                "' has an invalid attribute '" + attribute->getType() + "'.");
            err->setModel(model);
            err->setKind(Error::Kind::CONNECTION);
            mParser->addError(err);
            attribute = attribute->getNext();
        }
    }
    // Check that the connection node has children.
    XmlNodePtr childNode = node->getFirstChild();
    if (!childNode) {
        ErrorPtr err = std::make_shared<Error>();
        err->setDescription("Connection in model '" + model->getName() +
                            "' does not contain any child elements.");
        err->setModel(model);
        err->setKind(Error::Kind::CONNECTION);
        mParser->addError(err);
        return;
    }

    // Initialise name pairs and flags.
    NamePair componentNamePair, variableNamePair;
    NamePairMap variableNameMap;
    bool mapComponentsFound = false;
    bool mapVariablesFound = false;
    bool component1Missing = false;
    bool component2Missing = false;
    bool variable1Missing = false;
    bool variable2Missing = false;
    // Iterate over connection child XML nodes.
    while (childNode) {
        // Connection map XML nodes should not have further children.
        if (childNode->getFirstChild()) {
            XmlNodePtr grandchildNode = childNode->getFirstChild();
            if (grandchildNode->isType("text")) {
                std::string textNode = grandchildNode->convertToString();
                // Ignore whitespace when parsing.
                if (isNotWhitespace(textNode)) {
                    ErrorPtr err = std::make_shared<Error>();
                    err->setDescription("Connection in model '" + model->getName() +
                                        "' has an invalid non-whitespace child text element '" + textNode + "'.");
                    err->setModel(model);
                    err->setKind(Error::Kind::CONNECTION);
                    mParser->addError(err);
                }
            } else {
                ErrorPtr err = std::make_shared<Error>();
                err->setDescription("Connection in model '" + model->getName() +
                                    "' has an invalid child element '" + grandchildNode->getType() +
                                    "' of element '" + childNode->getType() + "'.");
                err->setModel(model);
                err->setKind(Error::Kind::CONNECTION);
                mParser->addError(err);
            }
        }

        // Check for a valid map_components and get the name pair.
        if (childNode->isType("map_components")) {
            std::string component1Name = "";
            std::string component2Name = "";
            XmlAttributePtr attribute = childNode->getFirstAttribute();
            while (attribute) {
                if (attribute->isType("component_1")) {
                    component1Name = attribute->getValue();
                } else if (attribute->isType("component_2")) {
                    component2Name = attribute->getValue();
                } else {
                    ErrorPtr err = std::make_shared<Error>();
                    err->setDescription("Connection in model '" + model->getName() +
                                        "' has an invalid map_components attribute '" + attribute->getType() + "'.");
                    err->setModel(model);
                    err->setKind(Error::Kind::CONNECTION);
                    mParser->addError(err);
                }
                attribute = attribute->getNext();
            }
            // Check that we found both components.
            if (!component1Name.length()) {
                ErrorPtr err = std::make_shared<Error>();
                err->setDescription("Connection in model '" + model->getName() +
                                    "' does not have a valid component_1 in a map_components element.");
                err->setModel(model);
                err->setKind(Error::Kind::CONNECTION);
                mParser->addError(err);
                component1Missing = true;
            }
            if (!component2Name.length()) {
                ErrorPtr err = std::make_shared<Error>();
                err->setDescription("Connection in model '" + model->getName() +
                                    "' does not have a valid component_2 in a map_components element.");
                err->setModel(model);
                err->setKind(Error::Kind::CONNECTION);
                mParser->addError(err);
                component2Missing = true;
            }
            // We should only have one map_components per connection.
            if (mapComponentsFound) {
                ErrorPtr err = std::make_shared<Error>();
                err->setDescription("Connection in model '" + model->getName() +
                                    "' has more than one map_components element.");
                err->setModel(model);
                err->setKind(Error::Kind::CONNECTION);
                mParser->addError(err);
            }
            componentNamePair = std::make_pair(component1Name, component2Name);
            mapComponentsFound = true;

        // Check for a valid map_variables and add the name pair to the variableNameMap.
        } else if (childNode->isType("map_variables")) {
            std::string variable1Name = "";
            std::string variable2Name = "";
            XmlAttributePtr attribute = childNode->getFirstAttribute();
            while (attribute) {
                if (attribute->isType("variable_1")) {
                    variable1Name = attribute->getValue();
                } else if (attribute->isType("variable_2")) {
                    variable2Name = attribute->getValue();
                } else {
                    ErrorPtr err = std::make_shared<Error>();
                    err->setDescription("Connection in model '" + model->getName() +
                                        "' has an invalid map_variables attribute '" + attribute->getType() + "'.");
                    err->setModel(model);
                    err->setKind(Error::Kind::CONNECTION);
                    mParser->addError(err);
                }
                attribute = attribute->getNext();
            }
            // Check that we found both variables.
            if (!variable1Name.length()) {
                ErrorPtr err = std::make_shared<Error>();
                err->setDescription("Connection in model '" + model->getName() +
                                    "' does not have a valid variable_1 in a map_variables element.");
                err->setModel(model);
                err->setKind(Error::Kind::CONNECTION);
                mParser->addError(err);
                variable1Missing = true;
            }
            if (!variable2Name.length()) {
                ErrorPtr err = std::make_shared<Error>();
                err->setDescription("Connection in model '" + model->getName() +
                                    "' does not have a valid variable_2 in a map_variables element.");
                err->setModel(model);
                err->setKind(Error::Kind::CONNECTION);
                mParser->addError(err);
                variable2Missing = true;
            }
            // We can have multiple map_variables per connection.
            variableNamePair = std::make_pair(variable1Name, variable2Name);
            variableNameMap.push_back(variableNamePair);
            mapVariablesFound = true;

        } else if (childNode->isType("text")) {
            std::string textNode = childNode->convertToString();
            // Ignore whitespace when parsing.
            if (isNotWhitespace(textNode)) {
                ErrorPtr err = std::make_shared<Error>();
                err->setDescription("Connection in model '" + model->getName() +
                                    "' has an invalid non-whitespace child text element '" + textNode + "'.");
                err->setModel(model);
                err->setKind(Error::Kind::CONNECTION);
                mParser->addError(err);
            }
        } else {
            ErrorPtr err = std::make_shared<Error>();
            err->setDescription("Connection in model '" + model->getName() +
                                "' has an invalid child element '" + childNode->getType() + "'.");
            err->setModel(model);
            err->setKind(Error::Kind::CONNECTION);
            mParser->addError(err);
        }
        childNode = childNode->getNext();
    }

    // If we have a map_components, check that the components exist in the model.
    ComponentPtr component1 = nullptr;
    ComponentPtr component2 = nullptr;
    if (mapComponentsFound) {
        // Now check the objects exist in the model.
        if (model->containsComponent(componentNamePair.first)) {
            component1 = model->getComponent(componentNamePair.first);
        } else {
            if (!component1Missing) {
                ErrorPtr err = std::make_shared<Error>();
                err->setDescription("Connection in model '" + model->getName() +
                                    "' specifies '" + componentNamePair.first +
                                    "' as component_1 but it does not exist in the model.");
                err->setModel(model);
                err->setKind(Error::Kind::CONNECTION);
                mParser->addError(err);
            }
        }
        if (model->containsComponent(componentNamePair.second)) {
            component2 = model->getComponent(componentNamePair.second);
        } else {
            if (!component2Missing) {
                ErrorPtr err = std::make_shared<Error>();
                err->setDescription("Connection in model '" + model->getName() +
                                    "' specifies '" + componentNamePair.second +
                                    "' as component_2 but it does not exist in the model.");
                err->setModel(model);
                err->setKind(Error::Kind::CONNECTION);
                mParser->addError(err);
            }
        }
    } else {
        ErrorPtr err = std::make_shared<Error>();
        err->setDescription("Connection in model '" + model->getName() +
                            "' does not have a map_components element.");
        err->setModel(model);
        err->setKind(Error::Kind::CONNECTION);
        mParser->addError(err);
    }

    // If we have a map_variables, check that the variables exist in the map_components.
    if (mapVariablesFound) {
        for (NameMapIterator iterPair = variableNameMap.begin(); iterPair < variableNameMap.end(); ++iterPair) {
            VariablePtr variable1 = nullptr;
            VariablePtr variable2 = nullptr;
            if (component1) {
                if (component1->hasVariable(iterPair->first)) {
                    variable1 = component1->getVariable(iterPair->first);
                } else if (component1->isImport()) {
                    // With an imported component we assume this variable exists in the imported component.
                    variable1 = std::make_shared<Variable>();
                    variable1->setName(iterPair->first);
                    component1->addVariable(variable1);
                } else {
                    if (!variable1Missing) {
                        ErrorPtr err = std::make_shared<Error>();
                        err->setDescription("Variable '" + iterPair->first +
                                            "' is specified as variable_1 in a connection but it does not exist in component_1 component '"
                                            + component1->getName() + "' of model '" + model->getName() + "'.");
                        err->setComponent(component1);
                        err->setKind(Error::Kind::CONNECTION);
                        mParser->addError(err);
                    }
                }
            } else {
                ErrorPtr err = std::make_shared<Error>();
                err->setDescription("Connection in model '" + model->getName() +
                                    "' specifies '" + iterPair->first +
                                    "' as variable_1 but the corresponding component_1 is invalid.");
                err->setModel(model);
                err->setKind(Error::Kind::CONNECTION);
                mParser->addError(err);
            }
            if (component2) {
                if (component2->hasVariable(iterPair->second)) {
                    variable2 = component2->getVariable(iterPair->second);
                } else if (component2->isImport()) {
                    // With an imported component we assume this variable exists in the imported component.
                    variable2 = std::make_shared<Variable>();
                    variable2->setName(iterPair->second);
                    component2->addVariable(variable2);
                } else {
                    if (!variable2Missing) {
                        ErrorPtr err = std::make_shared<Error>();
                        err->setDescription("Variable '" + iterPair->second +
                                            "' is specified as variable_2 in a connection but it does not exist in component_2 component '"
                                            + component2->getName() + "' of model '" + model->getName() + "'.");
                        err->setComponent(component1);
                        err->setKind(Error::Kind::CONNECTION);
                        mParser->addError(err);
                    }
                }
            } else {
                ErrorPtr err = std::make_shared<Error>();
                err->setDescription("Connection in model '" + model->getName() +
                                    "' specifies '" + iterPair->second +
                                    "' as variable_2 but the corresponding component_2 is invalid.");
                err->setModel(model);
                err->setKind(Error::Kind::CONNECTION);
                mParser->addError(err);
            }
            // Set the variable equivalence relationship for this variable pair.
            if ((variable1) && (variable2)) {
                Variable::addEquivalence(variable1, variable2);
            }
        }
    } else {
        ErrorPtr err = std::make_shared<Error>();
        err->setDescription("Connection in model '" + model->getName() +
                            "' does not have a map_variables element.");
        err->setModel(model);
        err->setKind(Error::Kind::CONNECTION);
        mParser->addError(err);
    }
}

void Parser::ParserImpl::loadEncapsulation(const ModelPtr &model, const XmlNodePtr &node)
{
    XmlNodePtr parentComponentNode = node;
    while (parentComponentNode) {
        ComponentPtr parentComponent = nullptr;
        std::string parentComponentName;
        if (parentComponentNode->isType("component_ref")) {
            // Check for a component in the parent component_ref.
            XmlAttributePtr attribute = parentComponentNode->getFirstAttribute();
            while (attribute) {
                if (attribute->isType("component")) {
                    parentComponentName = attribute->getValue();
                    if (model->containsComponent(parentComponentName)) {
                        // Will re-add this to the model once we encapsulate the child(ren).
                        parentComponent = model->takeComponent(parentComponentName);
                    } else {
                        ErrorPtr err = std::make_shared<Error>();
                        err->setDescription("Encapsulation in model '" + model->getName() +
                                            "' specifies '" + parentComponentName +
                                            "' as a component in a component_ref but it does not exist in the model.");
                        err->setModel(model);
                        err->setKind(Error::Kind::ENCAPSULATION);
                        mParser->addError(err);
                    }
                } else {
                    ErrorPtr err = std::make_shared<Error>();
                    err->setDescription("Encapsulation in model '" + model->getName() +
                                        "' has an invalid component_ref attribute '" + attribute->getType() + "'.");
                    err->setModel(model);
                    err->setKind(Error::Kind::ENCAPSULATION);
                    mParser->addError(err);
                }
                attribute = attribute->getNext();
            }
            if ((!parentComponent) && (!parentComponentName.length())) {
                ErrorPtr err = std::make_shared<Error>();
                err->setDescription("Encapsulation in model '" + model->getName() +
                                    "' does not have a valid component attribute in a component_ref element.");
                err->setModel(model);
                err->setKind(Error::Kind::ENCAPSULATION);
                mParser->addError(err);
            }
        } else if (parentComponentNode->isType("text")) {
            std::string textNode = parentComponentNode->convertToString();
            // Ignore whitespace when parsing.
            if (isNotWhitespace(textNode)) {
                ErrorPtr err = std::make_shared<Error>();
                err->setDescription("Encapsulation in model '" + model->getName() +
                                    "' has an invalid non-whitespace child text element '" + textNode + "'.");
                err->setModel(model);
                err->setKind(Error::Kind::ENCAPSULATION);
                mParser->addError(err);
            } else {
                // Continue to next node if this is whitespace (don't try to parse children of whitespace).
                parentComponentNode = parentComponentNode->getNext();
                continue;
            }
        } else {
            ErrorPtr err = std::make_shared<Error>();
            err->setDescription("Encapsulation in model '" + model->getName() +
                                "' has an invalid child element '" + parentComponentNode->getType() + "'.");
            err->setModel(model);
            err->setKind(Error::Kind::ENCAPSULATION);
            mParser->addError(err);
        }

        // Get first child of this parent component_ref.
        XmlNodePtr childComponentNode = parentComponentNode->getFirstChild();
        if (!childComponentNode) {
            ErrorPtr err = std::make_shared<Error>();
            if (parentComponent) {
                err->setDescription("Encapsulation in model '" + model->getName() +
                                    "' specifies '" + parentComponent->getName() +
                                    "' as a parent component_ref but it does not have any children.");
            } else {
                err->setDescription("Encapsulation in model '" + model->getName() +
                                    "' specifies an invalid parent component_ref that also does not have any children.");
            }
            err->setModel(model);
            err->setKind(Error::Kind::ENCAPSULATION);
            mParser->addError(err);
        }

        // Loop over encapsulated children.
        while (childComponentNode) {
            ComponentPtr childComponent = nullptr;
            if (childComponentNode->isType("component_ref")) {
                bool childComponentMissing = false;
                bool foundChildComponent = false;
                XmlAttributePtr attribute = childComponentNode->getFirstAttribute();
                while (attribute) {
                    if (attribute->isType("component")) {
                        std::string childComponentName = attribute->getValue();
                        if (model->containsComponent(childComponentName)) {
                            childComponent = model->getComponent(childComponentName);
                            foundChildComponent = true;
                        } else {
                            ErrorPtr err = std::make_shared<Error>();
                            err->setDescription("Encapsulation in model '" + model->getName() +
                                                "' specifies '" + childComponentName +
                                                "' as a component in a component_ref but it does not exist in the model.");
                            err->setModel(model);
                            err->setKind(Error::Kind::ENCAPSULATION);
                            mParser->addError(err);
                            childComponentMissing = true;
                        }
                    } else {
                        ErrorPtr err = std::make_shared<Error>();
                        err->setDescription("Encapsulation in model '" + model->getName() +
                                            "' has an invalid component_ref attribute '" + attribute->getType() + "'.");
                        err->setModel(model);
                        err->setKind(Error::Kind::ENCAPSULATION);
                        mParser->addError(err);
                    }
                    attribute = attribute->getNext();
                }
                if ((!foundChildComponent) && (!childComponentMissing)) {
                    ErrorPtr err = std::make_shared<Error>();
                    if (parentComponent) {
                        err->setDescription("Encapsulation in model '" + model->getName() +
                                            "' does not have a valid component attribute in a component_ref that is a child of '"
                                            + parentComponent->getName() + "'.");
                    } else if (parentComponentName.length()) {
                        err->setDescription("Encapsulation in model '" + model->getName() +
                                            "' does not have a valid component attribute in a component_ref that is a child of invalid parent component '"
                                            + parentComponentName + "'.");
                    } else {
                        err->setDescription("Encapsulation in model '" + model->getName() +
                                            "' does not have a valid component attribute in a component_ref that is a child of an invalid parent component.");
                    }
                    err->setModel(model);
                    err->setKind(Error::Kind::ENCAPSULATION);
                    mParser->addError(err);
                }

            } else if (childComponentNode->isType("text")) {
                std::string textNode = childComponentNode->convertToString();
                // Ignore whitespace when parsing.
                if (isNotWhitespace(textNode)) {
                    ErrorPtr err = std::make_shared<Error>();
                    err->setDescription("Encapsulation in model '" + model->getName() +
                                        "' has an invalid non-whitespace child text element '" + textNode + "'.");
                    err->setModel(model);
                    err->setKind(Error::Kind::ENCAPSULATION);
                    mParser->addError(err);
                }
            } else {
                ErrorPtr err = std::make_shared<Error>();
                err->setDescription("Encapsulation in model '" + model->getName() +
                                    "' has an invalid child element '" + childComponentNode->getType() + "'.");
                err->setModel(model);
                err->setKind(Error::Kind::ENCAPSULATION);
                mParser->addError(err);
            }

            if ((parentComponent) && (childComponent)) {
                // Set parent/child encapsulation relationship.
                parentComponent->addComponent(childComponent);
            }
            // Load any further encapsulated children.
            if (childComponentNode->getFirstChild()) {
                loadEncapsulation(model, childComponentNode);
            }
            if ((parentComponent) && (childComponent)) {
                // A child component is added through its parent component rather than the model,
                // so remove it if it exists.
                model->removeComponent(childComponent);
            }
            childComponentNode = childComponentNode->getNext();
        }

        // Re-add the parentComponent to the model with its child(ren) encapsulated.
        if (parentComponent) {
            model->addComponent(parentComponent);
        }
        // Get the next parent component at this level
        parentComponentNode = parentComponentNode->getNext();
    }
}

void Parser::ParserImpl::loadImport(const ImportPtr &import, const ModelPtr &model, const XmlNodePtr &node)
{
    XmlAttributePtr attribute = node->getFirstAttribute();
    while (attribute) {
        if (attribute->isType("href")) {
            import->setSource(attribute->getValue());
        } else if (attribute->isType("id")) {
            import->setId(attribute->getValue());
        } else if (attribute->isType("xlink")) {
            // Allow xlink attributes but do nothing for them.
        } else {
            ErrorPtr err = std::make_shared<Error>();
            err->setDescription("Import from '" + node->getAttribute("href") +
                                "' has an invalid attribute '" + attribute->getType() + "'.");
            err->setImport(import);
            err->setKind(Error::Kind::IMPORT);
            mParser->addError(err);
        }
        attribute = attribute->getNext();
    }
    XmlNodePtr childNode = node->getFirstChild();
    while (childNode) {
        if (childNode->isType("component")) {
            ComponentPtr importedComponent = std::make_shared<Component>();
            bool errorOccurred = false;
            XmlAttributePtr attribute = childNode->getFirstAttribute();
            while (attribute) {
                if (attribute->isType("name")) {
                    importedComponent->setName(attribute->getValue());
                } else if (attribute->isType("id")) {
                    importedComponent->setId(attribute->getValue());
                } else if (attribute->isType("component_ref")) {
                    importedComponent->setSourceComponent(import, attribute->getValue());
                } else {
                    ErrorPtr err = std::make_shared<Error>();
                    err->setDescription("Import of component '" + childNode->getAttribute("name") +
                                        "' from '" + node->getAttribute("href") +
                                        "' has an invalid attribute '" + attribute->getType() + "'.");
                    err->setImport(import);
                    err->setKind(Error::Kind::IMPORT);
                    mParser->addError(err);
                    errorOccurred = true;
                }
                attribute = attribute->getNext();
            }
            if (!errorOccurred) {
                model->addComponent(importedComponent);
            }
        } else if (childNode->isType("units")) {
            UnitsPtr importedUnits = std::make_shared<Units>();
            bool errorOccurred = false;
            XmlAttributePtr attribute = childNode->getFirstAttribute();
            while (attribute) {
                if (attribute->isType("name")) {
                    importedUnits->setName(attribute->getValue());
                } else if (attribute->isType("id")) {
                    importedUnits->setId(attribute->getValue());
                } else if (attribute->isType("units_ref")) {
                    importedUnits->setSourceUnits(import, attribute->getValue());
                } else {
                    ErrorPtr err = std::make_shared<Error>();
                    err->setDescription("Import of units '" + childNode->getAttribute("name") +
                                        "' from '" + node->getAttribute("href") +
                                        "' has an invalid attribute '" + attribute->getType() + "'.");
                    err->setImport(import);
                    err->setKind(Error::Kind::IMPORT);
                    mParser->addError(err);
                    errorOccurred = true;
                }
                attribute = attribute->getNext();
            }
            if (!errorOccurred) {
                model->addUnits(importedUnits);
            }
        } else if (childNode->isType("text")) {
            std::string textNode = childNode->convertToString();
            // Ignore whitespace when parsing.
            if (isNotWhitespace(textNode)) {
                ErrorPtr err = std::make_shared<Error>();
                err->setDescription("Import from '" + node->getAttribute("href") +
                                    "' has an invalid non-whitespace child text element '" + textNode + "'.");
                err->setImport(import);
                err->setKind(Error::Kind::IMPORT);
                mParser->addError(err);
            }
        } else {
            ErrorPtr err = std::make_shared<Error>();
            err->setDescription("Import from '" + node->getAttribute("href") +
                                "' has an invalid child element '" + childNode->getType() + "'.");
            err->setImport(import);
            err->setKind(Error::Kind::IMPORT);
            mParser->addError(err);
        }
        childNode = childNode->getNext();
    }
}

bool Parser::ParserImpl::isNotWhitespace (std::string &input)
{
    return input.find_first_not_of(" \t\n\v\f\r") != input.npos;
}

}