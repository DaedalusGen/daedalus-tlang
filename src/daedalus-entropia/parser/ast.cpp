#include <daedalus/Entropia/parser/ast.hpp>

#pragma region DeclarationExpression

daedalus::entropia::ast::DeclarationExpression::DeclarationExpression(std::shared_ptr<Identifier> identifier, std::shared_ptr<daedalus::core::ast::Expression> value, std::string value_type, bool isMutable) :
	AssignationExpression(identifier, value),
	value_type(value_type),
	isMutable(isMutable)
{}

std::string daedalus::entropia::ast::DeclarationExpression::get_value_type() {
	return this->value_type;
}
bool daedalus::entropia::ast::DeclarationExpression::get_mutability() {
	return this->isMutable;
}

std::string daedalus::entropia::ast::DeclarationExpression::type() {
	return "DeclarationExpression";
}
std::shared_ptr<daedalus::core::ast::Expression> daedalus::entropia::ast::DeclarationExpression::get_constexpr() {
	this->value = this->value->get_constexpr();
	return std::dynamic_pointer_cast<daedalus::entropia::ast::DeclarationExpression>(this->shared_from_this());
}
std::string daedalus::entropia::ast::DeclarationExpression::repr(int indent) {
	return
		std::string(indent, '\t') + std::string(this->isMutable ? "let mut" : "let ") + "\n" +
		this->identifier->repr(indent) + "\n" +
		std::string(indent, '\t') + ": " + this->value_type + "\n" +
		this->value->repr(indent);
}

#pragma endregion

#pragma region AssignationExpression

daedalus::entropia::ast::AssignationExpression::AssignationExpression(
	std::shared_ptr<Identifier> identifier,
	std::shared_ptr<daedalus::core::ast::Expression> value
) :
	Expression(),
	identifier(identifier),
	value(value)
{}

std::shared_ptr<daedalus::entropia::ast::Identifier> daedalus::entropia::ast::AssignationExpression::get_identifier() {
	return this->identifier;
}
std::shared_ptr<daedalus::core::ast::Expression> daedalus::entropia::ast::AssignationExpression::get_value() {
	return this->value;
}

std::string daedalus::entropia::ast::AssignationExpression::type() {
	return "AssignationExpression";
}
std::shared_ptr<daedalus::core::ast::Expression> daedalus::entropia::ast::AssignationExpression::get_constexpr() {
	this->value = this->value->get_constexpr();
	return this->shared_from_this();
}
std::string daedalus::entropia::ast::AssignationExpression::repr(int indent) {
	return
		this->identifier->repr(indent) + "\n" +
		std::string(indent, '\t') + "=\n" +
		this->value->repr(indent);
}

#pragma endregion

#pragma region Identifier

daedalus::entropia::ast::Identifier::Identifier(std::string name) :
	Expression(),
	name(name)
{}

std::string daedalus::entropia::ast::Identifier::get_name() {
	return this->name;
}
std::string daedalus::entropia::ast::Identifier::type() {
	return "Identifier";
}
std::shared_ptr<daedalus::core::ast::Expression> daedalus::entropia::ast::Identifier::get_constexpr() {
	return this->shared_from_this();
}
std::string daedalus::entropia::ast::Identifier::repr(int indent) {
	return std::string(indent, '\t') + this->name;
}

#pragma endregion

#pragma region BooleanExpression

daedalus::entropia::ast::BooleanExpression::BooleanExpression(bool value) :
	Expression(),
	value(value)
{}

bool daedalus::entropia::ast::BooleanExpression::BooleanExpression::get_value() {
    // Needed by UnaryExpression::get_constexpr
	return this->value;
}

void daedalus::entropia::ast::BooleanExpression::BooleanExpression::set_value(bool value) {
    this->value = value;
}

std::string daedalus::entropia::ast::BooleanExpression::BooleanExpression::type() {
	return "BooleanExpression";
}
std::shared_ptr<daedalus::core::ast::Expression> daedalus::entropia::ast::BooleanExpression::get_constexpr() {
	return this->shared_from_this();
}
std::string daedalus::entropia::ast::BooleanExpression::repr(int indent) {
	return std::string(indent, '\t') + std::string(this->value ? "true" : "false");
}

#pragma endregion

#pragma region CharExpression

daedalus::entropia::ast::CharExpression::CharExpression(char value) :
	Expression(),
	value(value)
{}

char daedalus::entropia::ast::CharExpression::get_value() {
	return this->value;
}

std::string daedalus::entropia::ast::CharExpression::type() {
	return "CharExpression";
}
std::shared_ptr<daedalus::core::ast::Expression> daedalus::entropia::ast::CharExpression::get_constexpr() {
	return this->shared_from_this();
}
std::string daedalus::entropia::ast::CharExpression::repr(int indent) {
	return std::string(indent, '\t') + "'" + std::string(1, this->value) + "'";
}

#pragma endregion

#pragma region StrExpression

daedalus::entropia::ast::StrExpression::StrExpression(std::string value) :
	Expression(),
	value(value)
{}

std::string daedalus::entropia::ast::StrExpression::get_value() {
	return this->value;
}

std::string daedalus::entropia::ast::StrExpression::type() {
	return "StrExpression";
}
std::shared_ptr<daedalus::core::ast::Expression> daedalus::entropia::ast::StrExpression::get_constexpr() {
	return this->shared_from_this();
}
std::string daedalus::entropia::ast::StrExpression::repr(int indent) {
	return std::string(indent, '\t') + "\"" + this->value + "\"";
}

#pragma region ContainerExpression

std::shared_ptr<daedalus::entropia::ast::Identifier> daedalus::entropia::ast::ContainerExpression::get_inner_identifier() {
	return nullptr;
}

#pragma endregion

#pragma region UnaryExpression

daedalus::entropia::ast::UnaryExpression::UnaryExpression(
	std::shared_ptr<Expression> term,
	std::string operator_symbol
) :
	ContainerExpression(),
	term(term),
	operator_symbol(operator_symbol)
{}

std::shared_ptr<daedalus::core::ast::Expression> daedalus::entropia::ast::UnaryExpression::get_term() {
	return this->term;
}

std::string daedalus::entropia::ast::UnaryExpression::get_operator_symbol() {
	return this->operator_symbol;
}

std::shared_ptr<daedalus::entropia::ast::Identifier> daedalus::entropia::ast::UnaryExpression::get_inner_identifier() {

	std::shared_ptr<daedalus::core::ast::Expression> constexprTerm = this->term->get_constexpr();

	if(constexprTerm->type() == "UnaryExpression") {
		return std::dynamic_pointer_cast<daedalus::entropia::ast::UnaryExpression>(constexprTerm)->get_inner_identifier();
	}
	if(constexprTerm->type() == "BinaryExpression") {
		return std::dynamic_pointer_cast<daedalus::entropia::ast::BinaryExpression>(constexprTerm)->get_inner_identifier();
	}

	return constexprTerm->type() == "Identifier" ? std::dynamic_pointer_cast<daedalus::entropia::ast::Identifier>(constexprTerm) : nullptr;
}

std::string daedalus::entropia::ast::UnaryExpression::type() {
	return "UnaryExpression";
}
std::shared_ptr<daedalus::core::ast::Expression> daedalus::entropia::ast::UnaryExpression::get_constexpr() {
	this->term = this->term->get_constexpr();
	if(auto booleanExpression = std::dynamic_pointer_cast<BooleanExpression>(this->term)) {
		if(this->operator_symbol == "!") {
			booleanExpression->set_value(!booleanExpression->get_value());
			return booleanExpression;
		}
	}
	return this->shared_from_this();
}
std::string daedalus::entropia::ast::UnaryExpression::repr(int indent) {
	return
		std::string(indent, '\t') + "(\n" +
		std::string(indent + 1, '\t') + this->operator_symbol + "\n" +
		this->term->repr(indent + 1) + "\n" +
		std::string(indent, '\t') + ")";
}

#pragma endregion

#pragma region BinaryExpression

daedalus::entropia::ast::BinaryExpression::BinaryExpression(
	std::shared_ptr<daedalus::core::ast::Expression> left,
	std::string operator_symbol,
	std::shared_ptr<daedalus::core::ast::Expression> right
) :
	ContainerExpression(),
	left(left),
	operator_symbol(operator_symbol),
	right(right)
{}

std::shared_ptr<daedalus::core::ast::Expression> daedalus::entropia::ast::BinaryExpression::get_left() {
	return this->left;
}
std::string daedalus::entropia::ast::BinaryExpression::get_operator_symbol() {
	return this->operator_symbol;
}
std::shared_ptr<daedalus::core::ast::Expression> daedalus::entropia::ast::BinaryExpression::get_right() {
	return this->right;
}

std::shared_ptr<daedalus::entropia::ast::Identifier> daedalus::entropia::ast::BinaryExpression::get_inner_identifier() {
	std::shared_ptr<daedalus::entropia::ast::Identifier> leftIdentifier = this->left_contains_identifier();
	if(leftIdentifier != nullptr) {
		return leftIdentifier;
	}
	return this->right_contains_identifier();
}

std::string daedalus::entropia::ast::BinaryExpression::type() {
	return "BinaryExpression";
}
std::shared_ptr<daedalus::core::ast::Expression> daedalus::entropia::ast::BinaryExpression::get_constexpr() {

	this->left = this->left->get_constexpr();
	this->right = this->right->get_constexpr();

	if(left->type() == "NumberExpression" && right->type() == "NumberExpression") {
		auto leftNb = std::dynamic_pointer_cast<daedalus::core::ast::NumberExpression>(left);
		auto rightNb = std::dynamic_pointer_cast<daedalus::core::ast::NumberExpression>(right);
		if(this->operator_symbol == "+") {
			return std::make_shared<daedalus::core::ast::NumberExpression>(leftNb->get_value() + rightNb->get_value());
		}
		if(this->operator_symbol == "-") {
			return std::make_shared<daedalus::core::ast::NumberExpression>(leftNb->get_value() - rightNb->get_value());
		}
		if(this->operator_symbol == "*") {
			return std::make_shared<daedalus::core::ast::NumberExpression>(leftNb->get_value() * rightNb->get_value());
		}
		if(this->operator_symbol == "/") {
			DAE_ASSERT_TRUE(
				rightNb->get_value() != 0,
				std::runtime_error("Trying to divide by zero")
			)
			return std::make_shared<daedalus::core::ast::NumberExpression>(leftNb->get_value() / rightNb->get_value());
		}
		if(this->operator_symbol == "&&") {
			return std::make_shared<BooleanExpression>(leftNb->get_value() && rightNb->get_value());
		}
		if(this->operator_symbol == "||") {
			return std::make_shared<BooleanExpression>(leftNb->get_value() || rightNb->get_value());
		}
		throw std::runtime_error("Invalid operator for NumberExpression and NumberExpression");
	}
	if(left->type() == "BooleanExpression" && right->type() == "BooleanExpression") {
		auto leftBool = std::dynamic_pointer_cast<BooleanExpression>(left);
		auto rightBool = std::dynamic_pointer_cast<BooleanExpression>(right);
		if(this->operator_symbol == "&&") {
			return std::make_shared<BooleanExpression>(leftBool->get_value() && rightBool->get_value());
		}
		if(this->operator_symbol == "||") {
			return std::make_shared<BooleanExpression>(leftBool->get_value() || rightBool->get_value());
		}
		throw std::runtime_error("Invalid operator for BooleanExpression and BooleanExpression");
	}
	if(left->type() == "NumberExpression" && right->type() == "BooleanExpression") {
		auto leftNb = std::dynamic_pointer_cast<daedalus::core::ast::NumberExpression>(left);
		auto rightBool = std::dynamic_pointer_cast<BooleanExpression>(right);
		if(this->operator_symbol == "&&") {
			return std::make_shared<BooleanExpression>(leftNb->get_value() && rightBool->get_value());
		}
		if(this->operator_symbol == "||") {
			return std::make_shared<BooleanExpression>(leftNb->get_value() || rightBool->get_value());
		}
		throw std::runtime_error("Invalid operator for NumberExpression and BooleanExpression");
	}
	if(left->type() == "BooleanExpression" && right->type() == "NumberExpression") {
		auto leftBool = std::dynamic_pointer_cast<BooleanExpression>(left);
		auto rightNb = std::dynamic_pointer_cast<daedalus::core::ast::NumberExpression>(right);
		if(this->operator_symbol == "&&") {
			return std::make_shared<BooleanExpression>(leftBool->get_value() && rightNb->get_value());
		}
		if(this->operator_symbol == "||") {
			return std::make_shared<BooleanExpression>(leftBool->get_value() || rightNb->get_value());
		}
		throw std::runtime_error("Invalid operator for BooleanExpression and NumberExpression");
	}

	if(
		left->type() == "Identifier" || right->type() == "Identifier" ||
		left->type() == "BinaryExpression" || right->type() == "BinaryExpression" ||
		left->type() == "UnaryExpression" || right->type() == "UnaryExpression"
	) {
		return std::make_shared<daedalus::entropia::ast::BinaryExpression>(
			left,
			this->operator_symbol,
			right
		);
	}

	throw std::runtime_error("Invalid operands " + this->left->repr() + " " + this->right->repr());
}
std::string daedalus::entropia::ast::BinaryExpression::repr(int indent) {
	return
		this->left->repr(indent) + "\n" +
		std::string(indent, '\t') + this->operator_symbol + "\n" +
		this->right->repr(indent);
}

std::shared_ptr<daedalus::entropia::ast::Identifier> daedalus::entropia::ast::BinaryExpression::left_contains_identifier() {

	std::shared_ptr<daedalus::core::ast::Expression> constexprLeft = this->left->get_constexpr();

	if(constexprLeft->type() == "UnaryExpression") {
		return std::dynamic_pointer_cast<daedalus::entropia::ast::UnaryExpression>(constexprLeft)->get_inner_identifier();
	}
	if(constexprLeft->type() == "BinaryExpression") {
		return std::dynamic_pointer_cast<daedalus::entropia::ast::BinaryExpression>(constexprLeft)->get_inner_identifier();
	}

	return constexprLeft->type() == "Identifier" ? std::dynamic_pointer_cast<daedalus::entropia::ast::Identifier>(constexprLeft) : nullptr;
}
std::shared_ptr<daedalus::entropia::ast::Identifier> daedalus::entropia::ast::BinaryExpression::right_contains_identifier() {

	std::shared_ptr<daedalus::core::ast::Expression> constexprRight = this->right->get_constexpr();

	if(constexprRight->type() == "UnaryExpression") {
		return std::dynamic_pointer_cast<daedalus::entropia::ast::UnaryExpression>(constexprRight)->get_inner_identifier();
	}
	if(constexprRight->type() == "BinaryExpression") {
		return std::dynamic_pointer_cast<daedalus::entropia::ast::BinaryExpression>(constexprRight)->get_inner_identifier();
	}

	return constexprRight->type() == "Identifier" ? std::dynamic_pointer_cast<daedalus::entropia::ast::Identifier>(constexprRight) : nullptr;
}

#pragma endregion

#pragma region OrExpression

daedalus::entropia::ast::OrExpression::OrExpression(std::shared_ptr<daedalus::core::ast::Expression> value) :
    value(value)
{}

std::shared_ptr<daedalus::core::ast::Expression> daedalus::entropia::ast::OrExpression::get_value() {
    return this->value;
}

std::string daedalus::entropia::ast::OrExpression::type() {
    return "OrExpression";
}
std::shared_ptr<daedalus::core::ast::Expression> daedalus::entropia::ast::OrExpression::get_constexpr() {
    this->value = this->value->get_constexpr();
    return this->shared_from_this();
}
std::string daedalus::entropia::ast::OrExpression::repr(int indent) {
    return std::string(indent, '\t') + "or\n" + this->value->repr(indent + 1);
}

#pragma endregion

#pragma region Loop

daedalus::entropia::ast::LoopExpression::LoopExpression(
    std::vector<std::shared_ptr<daedalus::core::ast::Expression>> body,
    std::shared_ptr<daedalus::entropia::ast::OrExpression> orExpression
) :
    daedalus::core::ast::Scope(body),
    orExpression(orExpression)
{}

std::shared_ptr<daedalus::entropia::ast::OrExpression> daedalus::entropia::ast::LoopExpression::get_or_expression() {
    return this->orExpression;
}

std::string daedalus::entropia::ast::LoopExpression::type() {
    return "LoopExpression";
}

std::string daedalus::entropia::ast::LoopExpression::repr(int indent) {
    std::string pretty = std::string(indent, '\t') + "loop {\n";

	for(std::shared_ptr<daedalus::core::ast::Expression> expression : this->body) {
		pretty += expression->repr(indent + 1) + "\n";
	}

	pretty += std::string(indent, '\t') + "}";

	if(this->orExpression != nullptr) {
	    pretty += " " + this->orExpression->repr();
	}

	return pretty;
}

#pragma endregion

#pragma region WhileExpression

daedalus::entropia::ast::WhileExpression::WhileExpression(
    std::vector<std::shared_ptr<daedalus::core::ast::Expression>> body,
    std::shared_ptr<daedalus::core::ast::Expression> condition,
    std::shared_ptr<daedalus::entropia::ast::OrExpression> orExpression
) :
    daedalus::entropia::ast::LoopExpression(body, orExpression),
    condition(condition)
{}

std::shared_ptr<daedalus::core::ast::Expression> daedalus::entropia::ast::WhileExpression::get_condition() {
    return this->condition;
}

std::string daedalus::entropia::ast::WhileExpression::type() {
    return "WhileExpression";
}

std::string daedalus::entropia::ast::WhileExpression::repr(int indent) {
    std::string pretty = std::string(indent, '\t') + "while(\n";
    pretty += this->condition->repr(indent + 1) + "\n";
    pretty += std::string(indent, '\t') + ") {\n";

	for(std::shared_ptr<daedalus::core::ast::Expression> expression : this->body) {
		pretty += expression->repr(indent + 1) + "\n";
	}

	pretty += std::string(indent, '\t') + "}";

	if(this->orExpression != nullptr) {
	    pretty += " " + this->orExpression->repr();
	}

	return pretty;
}

#pragma endregion

#pragma region ForExpression

daedalus::entropia::ast::ForExpression::ForExpression(
    std::vector<std::shared_ptr<daedalus::core::ast::Expression>> body,
    std::shared_ptr<daedalus::core::ast::Expression> initial_expression,
    std::shared_ptr<daedalus::core::ast::Expression> condition,
    std::shared_ptr<daedalus::core::ast::Expression> update_expression,
    std::shared_ptr<daedalus::entropia::ast::OrExpression> orExpression
) :
    daedalus::entropia::ast::WhileExpression(body, condition, orExpression),
    initialExpression(initial_expression),
    updateExpression(update_expression)
{}

std::shared_ptr<daedalus::core::ast::Expression> daedalus::entropia::ast::ForExpression::get_initial_expression() {
    return this->initialExpression;
}

std::shared_ptr<daedalus::core::ast::Expression> daedalus::entropia::ast::ForExpression::get_update_expression() {
    return this->updateExpression;
}

std::string daedalus::entropia::ast::ForExpression::type() {
    return "ForExpression";
}

std::string daedalus::entropia::ast::ForExpression::repr(int indent) {
    std::string pretty = std::string(indent, '\t') + "for(\n";

    pretty += this->initialExpression->repr(indent + 1) + "\n" + std::string(indent + 1, '\t') + ";\n";
    pretty += this->condition->repr(indent + 1) + "\n" + std::string(indent + 1, '\t') + ";\n";
    pretty += this->updateExpression->repr(indent + 1) + "\n";
    pretty += std::string(indent, '\t') + ") {\n";

	for(std::shared_ptr<daedalus::core::ast::Expression> expression : this->body) {
		pretty += expression->repr(indent + 1) + "\n";
	}

	pretty += std::string(indent, '\t') + "}";

	if(this->orExpression != nullptr) {
	    pretty += " " + this->orExpression->repr();
	}

	return pretty;
}

#pragma endregion

#pragma region BreakExpression

daedalus::entropia::ast::BreakExpression::BreakExpression() {}

std::string daedalus::entropia::ast::BreakExpression::type() {
    return "BreakExpression";
}
std::shared_ptr<daedalus::core::ast::Expression> daedalus::entropia::ast::BreakExpression::get_constexpr() {
    return this->shared_from_this();
}
std::string daedalus::entropia::ast::BreakExpression::repr(int indent) {
    return std::string(indent, '\t') + "break;";
}

#pragma endregion

#pragma region ContinueExpression

daedalus::entropia::ast::ContinueExpression::ContinueExpression() {}

std::string daedalus::entropia::ast::ContinueExpression::type() {
    return "ContinueExpression";
}
std::shared_ptr<daedalus::core::ast::Expression> daedalus::entropia::ast::ContinueExpression::get_constexpr() {
    return this->shared_from_this();
}
std::string daedalus::entropia::ast::ContinueExpression::repr(int indent) {
    return std::string(indent, '\t') + "continue;";
}

#pragma endregion

#pragma region ConditionnalExpression

daedalus::entropia::ast::ConditionnalExpression::ConditionnalExpression(
    std::vector<std::shared_ptr<daedalus::core::ast::Expression>> body,
    std::shared_ptr<daedalus::core::ast::Expression> condition,
    std::shared_ptr<daedalus::entropia::ast::ConditionnalExpression> before
) :
    daedalus::core::ast::Scope(body),
    condition(condition),
    before(before)
{}

std::shared_ptr<daedalus::entropia::ast::ConditionnalExpression> daedalus::entropia::ast::ConditionnalExpression::get_before() {
    return this->before;
}

std::shared_ptr<daedalus::core::ast::Expression> daedalus::entropia::ast::ConditionnalExpression::get_condition() {
    return this->condition;
}

std::string daedalus::entropia::ast::ConditionnalExpression::type() {
    return "ConditionnalExpression";
}

std::string daedalus::entropia::ast::ConditionnalExpression::repr(int indent) {
    std::string pretty =
        std::string(indent, '\t') +
        std::string(this->before == nullptr ?
            "if (\n" + this->condition->repr(indent + 1) + "\n" + std::string(indent, '\t') + ")":
            this->condition == nullptr ?
                "else" :
                "else if (\n" + this->condition->repr(indent + 1) + "\n" + std::string(indent, '\t') + ")"
        ) + " {\n";

	for(std::shared_ptr<daedalus::core::ast::Expression> expression : this->body) {
		pretty += expression->repr(indent + 1) + "\n";
	}

	pretty += std::string(indent, '\t') + "}";

	return pretty;
}

#pragma endregion

#pragma region ConditionnalStructure

daedalus::entropia::ast::ConditionnalStructure::ConditionnalStructure(
    std::vector<std::shared_ptr<daedalus::entropia::ast::ConditionnalExpression>> expressions
) :
    expressions(expressions)
{}

std::vector<std::shared_ptr<daedalus::entropia::ast::ConditionnalExpression>> daedalus::entropia::ast::ConditionnalStructure::get_expressions() {
    return this->expressions;
}

std::string daedalus::entropia::ast::ConditionnalStructure::type() {
    return "ConditionnalStructure";
}
std::shared_ptr<daedalus::core::ast::Expression> daedalus::entropia::ast::ConditionnalStructure::get_constexpr() {
    std::vector<std::shared_ptr<daedalus::entropia::ast::ConditionnalExpression>> expressions;
    for(std::shared_ptr<daedalus::entropia::ast::ConditionnalExpression> conditionnalExpression : this->expressions) {
        expressions.push_back(
            std::dynamic_pointer_cast<daedalus::entropia::ast::ConditionnalExpression>(
                conditionnalExpression->get_constexpr()
            )
        );
    }
    this->expressions = expressions;
    return this->shared_from_this();
}
std::string daedalus::entropia::ast::ConditionnalStructure::repr(int indent) {
    std::string pretty = "";
    for(std::shared_ptr<daedalus::entropia::ast::ConditionnalExpression> expression : this->expressions) {
        pretty += expression->repr(indent) + "\n";
    }
    return pretty;
}

#pragma endregion
