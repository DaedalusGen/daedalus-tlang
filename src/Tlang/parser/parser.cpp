#include <AquIce/Tlang/parser/parser.hpp>

// TODO Remove this
#define T_ERROR_ASSERT(condition, error) \
if(!(condition)) { \
	throw error; \
}

void setup_parser(daedalus::parser::Parser& parser) {

	daedalus::parser::setup_parser(
		parser,
		{
			{
				"Identifier",
				daedalus::parser::make_node(&tlang::parser::parse_identifier, false)
			},
			{
				"BooleanExpression",
				daedalus::parser::make_node(&tlang::parser::parse_boolean_expression, false)
			},
			{
				"UnaryExpression",
				daedalus::parser::make_node(&tlang::parser::parse_unary_expression, false)
			},
			{
				"BinaryExpression",
				daedalus::parser::make_node(&tlang::parser::parse_binary_expression, false)
			},
			{
				"AssignationExpression",
				daedalus::parser::make_node(&tlang::parser::parse_assignation_expression, false)
			},
			{
				"DeclarationExpression",
				daedalus::parser::make_node(&tlang::parser::parse_declaration_expression)
			},
		}
	);

	daedalus::parser::demoteTopNode(parser, "NumberExpression");
}

std::shared_ptr<daedalus::ast::Expression> tlang::parser::parse_identifier(std::vector<daedalus::lexer::Token>& tokens) {
	if(peek(tokens).type == "IDENTIFIER") {
		return std::make_shared<tlang::ast::Identifier>(eat(tokens).value);
	}
	return daedalus::parser::parse_number_expression(tokens);
}

std::shared_ptr<daedalus::ast::Expression> tlang::parser::parse_boolean_expression(std::vector<daedalus::lexer::Token>& tokens) {
	if(peek(tokens).type == "BOOL") {
		return std::make_shared<tlang::ast::BooleanExpression>(eat(tokens).value == "true");
	}
	return tlang::parser::parse_identifier(tokens);
}

std::shared_ptr<daedalus::ast::Expression> tlang::parser::parse_unary_expression(std::vector<daedalus::lexer::Token>& tokens) {

	std::string operator_symbol = "";

	if(peek(tokens).type == "UNARY_OPERATOR") {
		operator_symbol = eat(tokens).value;
	} else if(peek(tokens).value == "-") {
		(void)eat(tokens);
		std::shared_ptr<daedalus::ast::Expression> term = parse_boolean_expression(tokens)->get_constexpr();
		if(term->type() == "NumberExpression") {
			std::shared_ptr<daedalus::ast::NumberExpression> numberExpression = std::dynamic_pointer_cast<daedalus::ast::NumberExpression>(term);
			numberExpression->value *= -1;
			return numberExpression;
		}
		throw std::runtime_error("Invalid or not supported negative term");
	}
	
	std::shared_ptr<daedalus::ast::Expression> term = tlang::parser::parse_boolean_expression(tokens);

	return operator_symbol.size() == 0 ? term : std::make_shared<tlang::ast::UnaryExpression>(term, operator_symbol);
}

std::shared_ptr<daedalus::ast::Expression> tlang::parser::parse_multiplicative_expression(std::vector<daedalus::lexer::Token>& tokens) {
	std::shared_ptr<daedalus::ast::Expression> left = tlang::parser::parse_unary_expression(tokens);

	if(left == nullptr) {
		return nullptr;
	}

	if(
		peek(tokens).type == "BINARY_OPERATOR" &&
		(peek(tokens).value == "*" || peek(tokens).value == "/")
	) {
		std::string operator_symbol = eat(tokens).value;
		std::shared_ptr<daedalus::ast::Expression> right = tlang::parser::parse_multiplicative_expression(tokens);
		
		return std::make_shared<tlang::ast::BinaryExpression>(
			left,
			operator_symbol,
			right
		);
	}
	
	return left;
}

std::shared_ptr<daedalus::ast::Expression> tlang::parser::parse_additive_expression(std::vector<daedalus::lexer::Token>& tokens) {
	std::shared_ptr<daedalus::ast::Expression> left = tlang::parser::parse_multiplicative_expression(tokens);

	if(left == nullptr) {
		return nullptr;
	}

	if(
		peek(tokens).type == "BINARY_OPERATOR" &&
		(peek(tokens).value == "+" || peek(tokens).value == "-")
	) {
		std::string operator_symbol = eat(tokens).value;
		std::shared_ptr<daedalus::ast::Expression> right = tlang::parser::parse_additive_expression(tokens);
		
		return std::make_shared<tlang::ast::BinaryExpression>(
			left,
			operator_symbol,
			right
		);
	}
	
	return left;
}

std::shared_ptr<daedalus::ast::Expression> tlang::parser::parse_logical_expression(std::vector<daedalus::lexer::Token>& tokens) {
	std::shared_ptr<daedalus::ast::Expression> left = tlang::parser::parse_additive_expression(tokens);

	if(left == nullptr) {
		return nullptr;
	}

	if(
		peek(tokens).type == "BINARY_OPERATOR" &&
		(peek(tokens).value == "&&" || peek(tokens).value == "||")
	) {
		std::string operator_symbol = eat(tokens).value;
		std::shared_ptr<daedalus::ast::Expression> right = tlang::parser::parse_logical_expression(tokens);
		
		return std::make_shared<tlang::ast::BinaryExpression>(
			left,
			operator_symbol,
			right
		);
	}
	
	return left;
}

std::shared_ptr<daedalus::ast::Expression> tlang::parser::parse_binary_expression(std::vector<daedalus::lexer::Token>& tokens) {
	return tlang::parser::parse_logical_expression(tokens);
}

std::shared_ptr<daedalus::ast::Statement> tlang::parser::parse_assignation_expression(std::vector<daedalus::lexer::Token>& tokens) {
	
	std::shared_ptr<daedalus::ast::Expression> pseudoIdentifier = tlang::parser::parse_identifier(tokens);

	if(pseudoIdentifier->type() != "Identifier") {
		return tlang::parser::parse_binary_expression(tokens);
	}

	std::shared_ptr<tlang::ast::Identifier> identifier = std::dynamic_pointer_cast<tlang::ast::Identifier>(pseudoIdentifier);

	if(peek(tokens).type != "ASSIGN") {
		return tlang::parser::parse_identifier(tokens);
	}
	(void)eat(tokens);

	std::shared_ptr<daedalus::ast::Expression> expression = tlang::parser::parse_binary_expression(tokens);

	return std::make_shared<tlang::ast::AssignationExpression>(identifier, expression);
}

std::shared_ptr<daedalus::ast::Statement> tlang::parser::parse_declaration_expression(std::vector<daedalus::lexer::Token>& tokens) {

	std::cout << repr(peek(tokens)) << std::endl;
	if(peek(tokens).type != "ASSIGN_KEYWORD") {
		return parse_assignation_expression(tokens);
	}

	bool isMutable = eat(tokens).value == "let";
	
	std::shared_ptr<daedalus::ast::Expression> pseudoIdentifier = tlang::parser::parse_identifier(tokens);

	if(pseudoIdentifier->type() != "Identifier") {
		throw std::runtime_error("Expected identifier");
	}

	std::shared_ptr<tlang::ast::Identifier> identifier = std::dynamic_pointer_cast<tlang::ast::Identifier>(pseudoIdentifier);

	if(peek(tokens).type != "COLON") {
		throw std::runtime_error("Expected colon");
	}
	(void)eat(tokens);

	if(peek(tokens).type != "TYPE") {
		throw std::runtime_error("Expected type specifier");
	}
	std::string type = eat(tokens).value;

	if(peek(tokens).type != "ASSIGN") {
		throw std::runtime_error("Expected assignment symbol");
	}
	(void)eat(tokens);

	std::shared_ptr<daedalus::ast::Expression> expression = tlang::parser::parse_binary_expression(tokens)->get_constexpr();

	if(std::shared_ptr<tlang::ast::ContainerExpression> containerExpression = std::dynamic_pointer_cast<tlang::ast::ContainerExpression>(expression)) {
		if(std::dynamic_pointer_cast<tlang::ast::ContainerExpression>(expression)->contains_identifier()) {
			return std::make_shared<tlang::ast::DeclarationExpression>(identifier, expression, type, isMutable);
		}
	}

	// TODO Add i + u types to interpreter

	if(type == "i8") {
		T_ERROR_ASSERT(
			expression->type() == "NumberExpression",
			std::runtime_error("Expected valid i8 value (" + std::to_string(_I8_MIN) + " - " + std::to_string(_I8_MAX) + "), got " + expression->repr())
		)
		std::shared_ptr<daedalus::ast::NumberExpression> numberExpression = std::dynamic_pointer_cast<daedalus::ast::NumberExpression>(expression);
		T_ERROR_ASSERT(
			(numberExpression->value >= _I8_MIN && numberExpression->value <= _I8_MAX),
			std::runtime_error("Expected valid i8 value (" + std::to_string(_I8_MIN) + " - " + std::to_string(_I8_MAX) + "), got " + std::to_string(numberExpression->value))
		)
	}
	else if(type == "i16") {
		T_ERROR_ASSERT(
			expression->type() == "NumberExpression",
			std::runtime_error("Expected valid i16 value (" + std::to_string(_I16_MIN) + " - " + std::to_string(_I16_MAX) + "), got " + expression->repr())
		)
		std::shared_ptr<daedalus::ast::NumberExpression> numberExpression = std::dynamic_pointer_cast<daedalus::ast::NumberExpression>(expression);
		T_ERROR_ASSERT(
			(numberExpression->value >= _I16_MIN && numberExpression->value <= _I16_MAX),
			std::runtime_error("Expected valid i16 value (" + std::to_string(_I16_MIN) + " - " + std::to_string(_I16_MAX) + "), got " + std::to_string(numberExpression->value))
		)
	}
	else if(type == "i32") {
		T_ERROR_ASSERT(
			expression->type() == "NumberExpression",
			std::runtime_error("Expected valid i32 value (" + std::to_string(_I32_MIN) + " - " + std::to_string(_I32_MAX) + "), got " + expression->repr())
		)
		std::shared_ptr<daedalus::ast::NumberExpression> numberExpression = std::dynamic_pointer_cast<daedalus::ast::NumberExpression>(expression);
		T_ERROR_ASSERT(
			(numberExpression->value >= _I32_MIN && numberExpression->value <= _I32_MAX),
			std::runtime_error("Expected valid i32 value (" + std::to_string(_I32_MIN) + " - " + std::to_string(_I32_MAX) + "), got " + std::to_string(numberExpression->value))
		)
	}
	else if(type == "i64") {
		T_ERROR_ASSERT(
			expression->type() == "NumberExpression",
			std::runtime_error("Expected valid i64 value (" + std::to_string(_I64_MIN) + " - " + std::to_string(_I64_MAX) + "), got " + expression->repr())
		)
		std::shared_ptr<daedalus::ast::NumberExpression> numberExpression = std::dynamic_pointer_cast<daedalus::ast::NumberExpression>(expression);
		T_ERROR_ASSERT(
			(numberExpression->value >= _I64_MIN && numberExpression->value <= _I64_MAX),
			std::runtime_error("Expected valid i64 value (" + std::to_string(_I64_MIN) + " - " + std::to_string(_I64_MAX) + "), got " + std::to_string(numberExpression->value))
		)
	}
	else if(type == "u8") {
		T_ERROR_ASSERT(
			expression->type() == "NumberExpression",
			std::runtime_error("Expected valid u8 value (0 - " + std::to_string(_UI8_MAX) + "), got " + expression->repr())
		)
		std::shared_ptr<daedalus::ast::NumberExpression> numberExpression = std::dynamic_pointer_cast<daedalus::ast::NumberExpression>(expression);
		T_ERROR_ASSERT(
			(numberExpression->value >= 0 && numberExpression->value <= _UI8_MAX),
			std::runtime_error("Expected valid u8 value (0 - " + std::to_string(_UI8_MAX) + "), got " + std::to_string(numberExpression->value))
		)
	}
	else if(type == "u16") {
		T_ERROR_ASSERT(
			expression->type() == "NumberExpression",
			std::runtime_error("Expected valid u16 value (0 - " + std::to_string(_UI16_MAX) + "), got " + expression->repr())
		)
		std::shared_ptr<daedalus::ast::NumberExpression> numberExpression = std::dynamic_pointer_cast<daedalus::ast::NumberExpression>(expression);
		T_ERROR_ASSERT(
			(numberExpression->value >= 0 && numberExpression->value <= _UI16_MAX),
			std::runtime_error("Expected valid u16 value (0 - " + std::to_string(_UI16_MAX) + "), got " + std::to_string(numberExpression->value))
		)
	}
	else if(type == "u32") {
		T_ERROR_ASSERT(
			expression->type() == "NumberExpression",
			std::runtime_error("Expected valid u32 value (0 - " + std::to_string(_UI32_MAX) + "), got " + expression->repr())
		)
		std::shared_ptr<daedalus::ast::NumberExpression> numberExpression = std::dynamic_pointer_cast<daedalus::ast::NumberExpression>(expression);
		T_ERROR_ASSERT(
			(numberExpression->value >= 0 && numberExpression->value <= _UI32_MAX),
			std::runtime_error("Expected valid u32 value (0 - " + std::to_string(_UI32_MAX) + "), got " + std::to_string(numberExpression->value))
		)
	}
	else if(type == "u64") {
		T_ERROR_ASSERT(
			expression->type() == "NumberExpression",
			std::runtime_error("Expected valid u64 value (0 - " + std::to_string(_UI64_MAX) + "), got " + expression->repr())
		)
		std::shared_ptr<daedalus::ast::NumberExpression> numberExpression = std::dynamic_pointer_cast<daedalus::ast::NumberExpression>(expression);
		T_ERROR_ASSERT(
			(numberExpression->value >= 0 && numberExpression->value <= _UI64_MAX),
			std::runtime_error("Expected valid u64 value (0 - " + std::to_string(_UI64_MAX) + "), got " + std::to_string(numberExpression->value))
		)
	}
	else if(type == "bool") {
		T_ERROR_ASSERT(
			expression->type() == "BooleanExpression",
			std::runtime_error("Expected valid boolean value (true / false), got " + expression->repr())
		)
	}
	// TODO Add F32 + F64

	return std::make_shared<tlang::ast::DeclarationExpression>(identifier, expression, type, isMutable);
}