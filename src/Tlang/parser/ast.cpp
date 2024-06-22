#include <AquIce/Tlang/parser/ast.hpp>

#pragma region DeclarationExpression

tlang::ast::DeclarationExpression::DeclarationExpression(std::shared_ptr<Identifier> identifier, std::shared_ptr<daedalus::ast::Expression> value, std::string value_type, bool isMutable)
: AssignationExpression(identifier, value) {
	this->value_type = value_type;
	this->isMutable = isMutable;
}

std::string tlang::ast::DeclarationExpression::get_value_type() {
	return this->value_type;
}
bool tlang::ast::DeclarationExpression::get_mutability() {
	return this->isMutable;
}

std::string tlang::ast::DeclarationExpression::type() {
	return "DeclarationExpression";
}
std::shared_ptr<daedalus::ast::Expression> tlang::ast::DeclarationExpression::get_constexpr() {
	this->value = this->value->get_constexpr();
	return std::dynamic_pointer_cast<tlang::ast::DeclarationExpression>(this->shared_from_this());
}
std::string tlang::ast::DeclarationExpression::repr(int indent) {
	return
		std::string(indent, '\t') + std::string(this->isMutable ? "let " : "const ") + "\n" +
		this->identifier->repr(indent) + "\n" +
		std::string(indent, '\t') + ": " + this->value_type + "\n" +
		this->value->repr(indent);
}

#pragma endregion

#pragma region AssignationExpression

tlang::ast::AssignationExpression::AssignationExpression(
	std::shared_ptr<Identifier> identifier,
	std::shared_ptr<daedalus::ast::Expression> value
) {
	this->identifier = identifier;
	this->value = value;
}

std::shared_ptr<tlang::ast::Identifier> tlang::ast::AssignationExpression::get_identifier() {
	return this->identifier;
}
std::shared_ptr<daedalus::ast::Expression> tlang::ast::AssignationExpression::get_value() {
	return this->value;
}

std::string tlang::ast::AssignationExpression::type() {
	return "AssignationExpression";
}
std::shared_ptr<daedalus::ast::Expression> tlang::ast::AssignationExpression::get_constexpr() {
	this->value = this->value->get_constexpr();
	return this->shared_from_this();
}
std::string tlang::ast::AssignationExpression::repr(int indent) {
	return
		this->identifier->repr(indent) + "\n" +
		std::string(indent, '\t') + "=\n" +
		this->value->repr(indent);
}

#pragma endregion

#pragma region Identifier

tlang::ast::Identifier::Identifier(std::string name) {
	this->name = name;
}

std::string tlang::ast::Identifier::get_name() {
	return this->name;
}
std::string tlang::ast::Identifier::type() {
	return "Identifier";
}
std::shared_ptr<daedalus::ast::Expression> tlang::ast::Identifier::get_constexpr() {
	return this->shared_from_this();
}
std::string tlang::ast::Identifier::repr(int indent) {
	return std::string(indent, '\t') + this->name;
}

#pragma endregion

#pragma region BooleanExpression

tlang::ast::BooleanExpression::BooleanExpression(bool value) {
	this->value = value;
}

std::string tlang::ast::BooleanExpression::BooleanExpression::type() {
	return "BooleanExpression";
}
std::shared_ptr<daedalus::ast::Expression> tlang::ast::BooleanExpression::get_constexpr() {
	return this->shared_from_this();
}
std::string tlang::ast::BooleanExpression::repr(int indent) {
	return std::string(indent, '\t') + std::string(this->value ? "true" : "false");
}

#pragma endregion

#pragma region UnaryExpression

tlang::ast::UnaryExpression::UnaryExpression(
	std::shared_ptr<Expression> term,
	std::string operator_symbol
) {
	this->term = term;
	this->operator_symbol = operator_symbol;
}

std::shared_ptr<daedalus::ast::Expression> tlang::ast::UnaryExpression::get_term() {
	return this->term;
}

std::string tlang::ast::UnaryExpression::get_operator_symbol() {
	return this->operator_symbol;
}

std::string tlang::ast::UnaryExpression::type() {
	return "UnaryExpression";
}
std::shared_ptr<daedalus::ast::Expression> tlang::ast::UnaryExpression::get_constexpr() {
	this->term = this->term->get_constexpr();
	if(std::shared_ptr<BooleanExpression> booleanExpression = std::dynamic_pointer_cast<BooleanExpression>(this->term)) {
		if(this->operator_symbol == "!") {
			booleanExpression->value = !booleanExpression->value;
			return booleanExpression;
		}
	}
	return this->shared_from_this();
}
std::string tlang::ast::UnaryExpression::repr(int indent) {
	return
		std::string(indent, '\t') + "(\n" +
		std::string(indent + 1, '\t') + this->operator_symbol + "\n" +
		this->term->repr(indent + 1) + "\n" +
		std::string(indent, '\t') + ")";
}

#pragma endregion

#pragma region BinaryExpression

tlang::ast::BinaryExpression::BinaryExpression(
	std::shared_ptr<daedalus::ast::Expression> left,
	std::string operator_symbol,
	std::shared_ptr<daedalus::ast::Expression> right
) {
	this->left = left;
	this->operator_symbol = operator_symbol;
	this->right = right;
}

std::shared_ptr<daedalus::ast::Expression> tlang::ast::BinaryExpression::get_left() {
	return this->left;
}
std::string tlang::ast::BinaryExpression::get_operator_symbol() {
	return this->operator_symbol;
}
std::shared_ptr<daedalus::ast::Expression> tlang::ast::BinaryExpression::get_right() {
	return this->right;
}

std::string tlang::ast::BinaryExpression::type() {
	return "BinaryExpression";
}
std::shared_ptr<daedalus::ast::Expression> tlang::ast::BinaryExpression::get_constexpr() {

	this->left = this->left->get_constexpr();
	this->right = this->right->get_constexpr();

	if(left->type() == "NumberExpression" && right->type() == "NumberExpression") {
		std::shared_ptr<daedalus::ast::NumberExpression> leftNb = std::dynamic_pointer_cast<daedalus::ast::NumberExpression>(left);
		std::shared_ptr<daedalus::ast::NumberExpression> rightNb = std::dynamic_pointer_cast<daedalus::ast::NumberExpression>(right);
		if(this->operator_symbol == "+") {
			return std::make_shared<daedalus::ast::NumberExpression>(leftNb->value + rightNb->value);
		}
		if(this->operator_symbol == "-") {
			return std::make_shared<daedalus::ast::NumberExpression>(leftNb->value - rightNb->value);
		}
		if(this->operator_symbol == "*") {
			return std::make_shared<daedalus::ast::NumberExpression>(leftNb->value * rightNb->value);
		}
		if(this->operator_symbol == "/") {
			if(rightNb->value == 0) {
				throw std::runtime_error("Trying to divide by zero");
			}
			return std::make_shared<daedalus::ast::NumberExpression>(leftNb->value / rightNb->value);
		}
		if(this->operator_symbol == "&&") {
			return std::make_shared<BooleanExpression>(leftNb->value && rightNb->value);
		}
		if(this->operator_symbol == "||") {
			return std::make_shared<BooleanExpression>(leftNb->value || rightNb->value);
		}
		throw std::runtime_error("Invalid operator for NumberExpression and NumberExpression");
	}
	if(left->type() == "BooleanExpression" && right->type() == "BooleanExpression") {
		std::shared_ptr<BooleanExpression> leftBool = std::dynamic_pointer_cast<BooleanExpression>(left);
		std::shared_ptr<BooleanExpression> rightBool = std::dynamic_pointer_cast<BooleanExpression>(right);
		if(this->operator_symbol == "&&") {
			return std::make_shared<BooleanExpression>(leftBool->value && rightBool->value);
		}
		if(this->operator_symbol == "||") {
			return std::make_shared<BooleanExpression>(leftBool->value || rightBool->value);
		}
		throw std::runtime_error("Invalid operator for BooleanExpression and BooleanExpression");
	}
	if(left->type() == "NumberExpression" && right->type() == "BooleanExpression") {
		std::shared_ptr<daedalus::ast::NumberExpression> leftNb = std::dynamic_pointer_cast<daedalus::ast::NumberExpression>(left);
		std::shared_ptr<BooleanExpression> rightBool = std::dynamic_pointer_cast<BooleanExpression>(right);
		if(this->operator_symbol == "&&") {
			return std::make_shared<BooleanExpression>(leftNb->value && rightBool->value);
		}
		if(this->operator_symbol == "||") {
			return std::make_shared<BooleanExpression>(leftNb->value || rightBool->value);
		}
		throw std::runtime_error("Invalid operator for NumberExpression and BooleanExpression");
	}
	if(left->type() == "BooleanExpression" && right->type() == "NumberExpression") {
		std::shared_ptr<BooleanExpression> leftBool = std::dynamic_pointer_cast<BooleanExpression>(left);
		std::shared_ptr<daedalus::ast::NumberExpression> rightNb = std::dynamic_pointer_cast<daedalus::ast::NumberExpression>(right);
		if(this->operator_symbol == "&&") {
			return std::make_shared<BooleanExpression>(leftBool->value && rightNb->value);
		}
		if(this->operator_symbol == "||") {
			return std::make_shared<BooleanExpression>(leftBool->value || rightNb->value);
		}
		throw std::runtime_error("Invalid operator for BooleanExpression and NumberExpression");
	}
	
	if(left->type() == "Identifier" || left->type() == "Identifier") {
		return std::make_shared<tlang::ast::BinaryExpression>(
			left,
			this->operator_symbol,
			right
		);
	}

	throw std::runtime_error("Invalid operands " + this->left->repr() + " " + this->right->repr());
}
std::string tlang::ast::BinaryExpression::repr(int indent) {
	return
		std::string(indent, '\t') + "(\n" +
		this->left->repr(indent + 1) + "\n" +
		std::string(indent + 1, '\t') + this->operator_symbol + "\n" +
		this->right->repr(indent + 1) + "\n" +
		std::string(indent, '\t') + ")";
}

#pragma endregion