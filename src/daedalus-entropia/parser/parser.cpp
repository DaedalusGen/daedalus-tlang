#include <daedalus/Entropia/parser/parser.hpp>

void setup_parser(daedalus::core::parser::Parser& parser) {

	daedalus::core::parser::setup_parser(
		parser,
		{
			{
				"Identifier",
				daedalus::core::parser::make_node(&daedalus::entropia::parser::parse_identifier, false)
			},
			{
				"BooleanExpression",
				daedalus::core::parser::make_node(&daedalus::entropia::parser::parse_boolean_expression, false)
			},
			{
				"CharExpression",
				daedalus::core::parser::make_node(&daedalus::entropia::parser::parse_char_expression, false)
			},
			{
				"StrExpression",
				daedalus::core::parser::make_node(&daedalus::entropia::parser::parse_str_expression, false)
			},
			{
				"UnaryExpression",
				daedalus::core::parser::make_node(&daedalus::entropia::parser::parse_unary_expression, false)
			},
			{
				"BinaryExpression",
				daedalus::core::parser::make_node(&daedalus::entropia::parser::parse_binary_expression, false)
			},
			{
				"AssignationExpression",
				daedalus::core::parser::make_node(&daedalus::entropia::parser::parse_assignation_expression, false)
			},
			{
				"DeclarationExpression",
				daedalus::core::parser::make_node(&daedalus::entropia::parser::parse_declaration_expression)
			},
		}
	);

	daedalus::core::parser::demoteTopNode(parser, "NumberExpression");
}

char get_char(std::string src) {
	if(src.at(0) == '\\') {
		switch(src.at(1)) {
			case 'n':
				return '\n';
			case 't':
				return '\t';
			case 'r':
				return '\r';
			case '\'':
				return '\'';
			case '\\':
				return '\\';
			default:
				throw std::runtime_error("Invalid escape sequence (should not have been validated by lexer)");
		}
	}
	return src.at(0);
}

std::unordered_map<std::string, std::string> daedalus::entropia::parser::identifiers = std::unordered_map<std::string, std::string>();

std::shared_ptr<daedalus::core::ast::Expression> daedalus::entropia::parser::parse_identifier(daedalus::core::parser::Parser& parser, std::vector<daedalus::core::lexer::Token>& tokens) {
	if(peek(tokens).type == "IDENTIFIER") {
		return std::make_shared<daedalus::entropia::ast::Identifier>(eat(tokens).value);
	}
	return daedalus::core::parser::parse_number_expression(parser, tokens);
}

std::shared_ptr<daedalus::core::ast::Expression> daedalus::entropia::parser::parse_boolean_expression(daedalus::core::parser::Parser& parser, std::vector<daedalus::core::lexer::Token>& tokens) {
	if(peek(tokens).type == "BOOL") {
		return std::make_shared<daedalus::entropia::ast::BooleanExpression>(eat(tokens).value == "true");
	}
	return daedalus::entropia::parser::parse_identifier(parser, tokens);
}

std::shared_ptr<daedalus::core::ast::Expression> daedalus::entropia::parser::parse_char_expression(daedalus::core::parser::Parser& parser, std::vector<daedalus::core::lexer::Token>& tokens) {
	if(peek(tokens).type == "CHAR") {
		std::string value = eat(tokens).value;
		return std::make_shared<daedalus::entropia::ast::CharExpression>(get_char(value.substr(1, value.length() - 2)));
	}
	return daedalus::entropia::parser::parse_identifier(parser, tokens);
}

std::shared_ptr<daedalus::core::ast::Expression> daedalus::entropia::parser::parse_str_expression(daedalus::core::parser::Parser& parser, std::vector<daedalus::core::lexer::Token>& tokens) {
	if(peek(tokens).type == "STR") {
		std::string str = "";
		size_t i = 0;
		std::string value = eat(tokens).value;
		value = value.substr(1, value.length() - 2);
		while(i < value.length()) {
			char c = get_char(value.substr(i));
			i += value.at(i) == '\\' ? 2 : 1;
			str += c;
		}
		return std::make_shared<daedalus::entropia::ast::StrExpression>(str);
	}
	return daedalus::entropia::parser::parse_char_expression(parser, tokens);
}

std::shared_ptr<daedalus::core::ast::Expression> daedalus::entropia::parser::parse_parenthesis_expression(daedalus::core::parser::Parser& parser, std::vector<daedalus::core::lexer::Token>& tokens) {
	if(peek(tokens).type != "OPEN_PAREN") {
		return parse_str_expression(parser, tokens);
	}
	(void)eat(tokens);

	std::shared_ptr<daedalus::core::ast::Statement> parenthesisStatement = daedalus::core::parser::parse_statement(parser, tokens);

	(void)expect(tokens, "CLOSE_PAREN", std::runtime_error("Expected closed parenthesis"));

	if(std::shared_ptr<daedalus::core::ast::Expression> parenthesisExpression = std::dynamic_pointer_cast<daedalus::core::ast::Expression>(parenthesisStatement)) {
		return parenthesisExpression;
	}

	throw std::runtime_error("Invalid parenthesis expression");
}

std::shared_ptr<daedalus::core::ast::Expression> daedalus::entropia::parser::parse_unary_expression(daedalus::core::parser::Parser& parser, std::vector<daedalus::core::lexer::Token>& tokens) {

	std::string operator_symbol = "";

	if(peek(tokens).type == "UNARY_OPERATOR") {
		operator_symbol = eat(tokens).value;
		DAE_ASSERT_TRUE(
			operator_symbol == "!",
			std::runtime_error("Invalid unary operator")
		)
	} else if(peek(tokens).value == "-") {
		(void)eat(tokens);
		std::shared_ptr<daedalus::core::ast::Expression> term = parse_parenthesis_expression(parser, tokens)->get_constexpr();
		if(term->type() == "Identifier") {
			return std::make_shared<daedalus::entropia::ast::BinaryExpression>(
				std::make_shared<daedalus::core::ast::NumberExpression>(0),
				"-",
				term
			);
		}
		DAE_ASSERT_TRUE(
			term->type() == "NumberExpression",
			std::runtime_error("Invalid or not supported negative term")
		)
		std::shared_ptr<daedalus::core::ast::NumberExpression> numberExpression = std::dynamic_pointer_cast<daedalus::core::ast::NumberExpression>(term);
		numberExpression->value *= -1;

		return numberExpression;
	}

	std::shared_ptr<daedalus::core::ast::Expression> term = daedalus::entropia::parser::parse_parenthesis_expression(parser, tokens);

	return operator_symbol.size() == 0 ? term : std::make_shared<daedalus::entropia::ast::UnaryExpression>(term, operator_symbol);
}

std::shared_ptr<daedalus::core::ast::Expression> daedalus::entropia::parser::parse_multiplicative_expression(daedalus::core::parser::Parser& parser, std::vector<daedalus::core::lexer::Token>& tokens) {
	std::shared_ptr<daedalus::core::ast::Expression> left = daedalus::entropia::parser::parse_unary_expression(parser, tokens);

	if(
		peek(tokens).type == "BINARY_OPERATOR" &&
		(peek(tokens).value == "*" || peek(tokens).value == "/")
	) {
		std::string operator_symbol = eat(tokens).value;
		std::shared_ptr<daedalus::core::ast::Expression> right = daedalus::entropia::parser::parse_multiplicative_expression(parser, tokens);

		return std::make_shared<daedalus::entropia::ast::BinaryExpression>(
			left,
			operator_symbol,
			right
		);
	}

	return left;
}

std::shared_ptr<daedalus::core::ast::Expression> daedalus::entropia::parser::parse_additive_expression(daedalus::core::parser::Parser& parser, std::vector<daedalus::core::lexer::Token>& tokens) {
	std::shared_ptr<daedalus::core::ast::Expression> left = daedalus::entropia::parser::parse_multiplicative_expression(parser, tokens);

	if(
		peek(tokens).type == "BINARY_OPERATOR" &&
		(peek(tokens).value == "+" || peek(tokens).value == "-")
	) {
		std::string operator_symbol = eat(tokens).value;
		std::shared_ptr<daedalus::core::ast::Expression> right = daedalus::entropia::parser::parse_additive_expression(parser, tokens);

		return std::make_shared<daedalus::entropia::ast::BinaryExpression>(
			left,
			operator_symbol,
			right
		);
	}

	return left;
}

std::shared_ptr<daedalus::core::ast::Expression> daedalus::entropia::parser::parse_logical_expression(daedalus::core::parser::Parser& parser, std::vector<daedalus::core::lexer::Token>& tokens) {
	std::shared_ptr<daedalus::core::ast::Expression> left = daedalus::entropia::parser::parse_additive_expression(parser, tokens);

	if(
		peek(tokens).type == "BINARY_OPERATOR" &&
		(peek(tokens).value == "&&" || peek(tokens).value == "||")
	) {
		std::string operator_symbol = eat(tokens).value;
		std::shared_ptr<daedalus::core::ast::Expression> right = daedalus::entropia::parser::parse_logical_expression(parser, tokens);

		return std::make_shared<daedalus::entropia::ast::BinaryExpression>(
			left,
			operator_symbol,
			right
		);
	}

	return left;
}

std::shared_ptr<daedalus::core::ast::Expression> daedalus::entropia::parser::parse_binary_expression(daedalus::core::parser::Parser& parser, std::vector<daedalus::core::lexer::Token>& tokens) {
	return daedalus::entropia::parser::parse_logical_expression(parser, tokens);
}

std::shared_ptr<daedalus::core::ast::Statement> daedalus::entropia::parser::parse_assignation_expression(daedalus::core::parser::Parser& parser, std::vector<daedalus::core::lexer::Token>& tokens) {

	std::shared_ptr<daedalus::core::ast::Expression> pseudoIdentifier = daedalus::entropia::parser::parse_binary_expression(parser, tokens);

	if(pseudoIdentifier->type() != "Identifier") {
		DAE_ASSERT_TRUE(
			eat(tokens).type == "SEMICOLON",
			std::runtime_error("Expected semicolon at the end of line")
		)
		return pseudoIdentifier;
	}

	std::shared_ptr<daedalus::entropia::ast::Identifier> identifier = std::dynamic_pointer_cast<daedalus::entropia::ast::Identifier>(pseudoIdentifier);

	if(peek(tokens).type != "ASSIGN") {
		return identifier;
	}
	(void)eat(tokens);

	std::shared_ptr<daedalus::core::ast::Expression> expression = daedalus::entropia::parser::parse_binary_expression(parser, tokens);

	if(std::shared_ptr<daedalus::entropia::ast::ContainerExpression> containerExpression = std::dynamic_pointer_cast<daedalus::entropia::ast::ContainerExpression>(expression)) {
		if(std::shared_ptr<daedalus::entropia::ast::Identifier> source_identifier = std::dynamic_pointer_cast<daedalus::entropia::ast::ContainerExpression>(expression)->contains_identifier()) {
			return std::make_shared<daedalus::entropia::ast::AssignationExpression>(identifier, expression);
		}
	}

	std::string type = daedalus::entropia::parser::identifiers.at(identifier->get_name());

	if(type == "i8") {
		DAE_ASSERT_TRUE(
			expression->type() == "NumberExpression",
			std::runtime_error("Expected valid i8 value (" + std::to_string(_I8_MIN) + " - " + std::to_string(_I8_MAX) + "), got " + expression->repr())
		)
		std::shared_ptr<daedalus::core::ast::NumberExpression> numberExpression = std::dynamic_pointer_cast<daedalus::core::ast::NumberExpression>(expression);
		DAE_ASSERT_TRUE(
			(numberExpression->value >= _I8_MIN && numberExpression->value <= _I8_MAX),
			std::runtime_error("Expected valid i8 value (" + std::to_string(_I8_MIN) + " - " + std::to_string(_I8_MAX) + "), got " + std::to_string(numberExpression->value))
		)
	}
	else if(type == "i16") {
		DAE_ASSERT_TRUE(
			expression->type() == "NumberExpression",
			std::runtime_error("Expected valid i16 value (" + std::to_string(_I16_MIN) + " - " + std::to_string(_I16_MAX) + "), got " + expression->repr())
		)
		std::shared_ptr<daedalus::core::ast::NumberExpression> numberExpression = std::dynamic_pointer_cast<daedalus::core::ast::NumberExpression>(expression);
		DAE_ASSERT_TRUE(
			(numberExpression->value >= _I16_MIN && numberExpression->value <= _I16_MAX),
			std::runtime_error("Expected valid i16 value (" + std::to_string(_I16_MIN) + " - " + std::to_string(_I16_MAX) + "), got " + std::to_string(numberExpression->value))
		)
	}
	else if(type == "i32") {
		DAE_ASSERT_TRUE(
			expression->type() == "NumberExpression",
			std::runtime_error("Expected valid i32 value (" + std::to_string(_I32_MIN) + " - " + std::to_string(_I32_MAX) + "), got " + expression->repr())
		)
		std::shared_ptr<daedalus::core::ast::NumberExpression> numberExpression = std::dynamic_pointer_cast<daedalus::core::ast::NumberExpression>(expression);
		DAE_ASSERT_TRUE(
			(numberExpression->value >= _I32_MIN && numberExpression->value <= _I32_MAX),
			std::runtime_error("Expected valid i32 value (" + std::to_string(_I32_MIN) + " - " + std::to_string(_I32_MAX) + "), got " + std::to_string(numberExpression->value))
		)
	}
	else if(type == "i64") {
		DAE_ASSERT_TRUE(
			expression->type() == "NumberExpression",
			std::runtime_error("Expected valid i64 value (" + std::to_string(_I64_MIN) + " - " + std::to_string(_I64_MAX) + "), got " + expression->repr())
		)
		std::shared_ptr<daedalus::core::ast::NumberExpression> numberExpression = std::dynamic_pointer_cast<daedalus::core::ast::NumberExpression>(expression);
		DAE_ASSERT_TRUE(
			(numberExpression->value >= _I64_MIN && numberExpression->value <= _I64_MAX),
			std::runtime_error("Expected valid i64 value (" + std::to_string(_I64_MIN) + " - " + std::to_string(_I64_MAX) + "), got " + std::to_string(numberExpression->value))
		)
	}
	else if(type == "u8") {
		DAE_ASSERT_TRUE(
			expression->type() == "NumberExpression",
			std::runtime_error("Expected valid u8 value (0 - " + std::to_string(_UI8_MAX) + "), got " + expression->repr())
		)
		std::shared_ptr<daedalus::core::ast::NumberExpression> numberExpression = std::dynamic_pointer_cast<daedalus::core::ast::NumberExpression>(expression);
		DAE_ASSERT_TRUE(
			(numberExpression->value >= 0 && numberExpression->value <= _UI8_MAX),
			std::runtime_error("Expected valid u8 value (0 - " + std::to_string(_UI8_MAX) + "), got " + std::to_string(numberExpression->value))
		)
	}
	else if(type == "u16") {
		DAE_ASSERT_TRUE(
			expression->type() == "NumberExpression",
			std::runtime_error("Expected valid u16 value (0 - " + std::to_string(_UI16_MAX) + "), got " + expression->repr())
		)
		std::shared_ptr<daedalus::core::ast::NumberExpression> numberExpression = std::dynamic_pointer_cast<daedalus::core::ast::NumberExpression>(expression);
		DAE_ASSERT_TRUE(
			(numberExpression->value >= 0 && numberExpression->value <= _UI16_MAX),
			std::runtime_error("Expected valid u16 value (0 - " + std::to_string(_UI16_MAX) + "), got " + std::to_string(numberExpression->value))
		)
	}
	else if(type == "u32") {
		DAE_ASSERT_TRUE(
			expression->type() == "NumberExpression",
			std::runtime_error("Expected valid u32 value (0 - " + std::to_string(_UI32_MAX) + "), got " + expression->repr())
		)
		std::shared_ptr<daedalus::core::ast::NumberExpression> numberExpression = std::dynamic_pointer_cast<daedalus::core::ast::NumberExpression>(expression);
		DAE_ASSERT_TRUE(
			(numberExpression->value >= 0 && numberExpression->value <= _UI32_MAX),
			std::runtime_error("Expected valid u32 value (0 - " + std::to_string(_UI32_MAX) + "), got " + std::to_string(numberExpression->value))
		)
	}
	else if(type == "u64") {
		DAE_ASSERT_TRUE(
			expression->type() == "NumberExpression",
			std::runtime_error("Expected valid u64 value (0 - " + std::to_string(_UI64_MAX) + "), got " + expression->repr())
		)
		std::shared_ptr<daedalus::core::ast::NumberExpression> numberExpression = std::dynamic_pointer_cast<daedalus::core::ast::NumberExpression>(expression);
		DAE_ASSERT_TRUE(
			(numberExpression->value >= 0 && numberExpression->value <= _UI64_MAX),
			std::runtime_error("Expected valid u64 value (0 - " + std::to_string(_UI64_MAX) + "), got " + std::to_string(numberExpression->value))
		)
	}
	else if(type == "f32") {
		DAE_ASSERT_TRUE(
			expression->type() == "NumberExpression",
			std::runtime_error("Expected valid f32 value (" + std::to_string(FLT_MIN) + " - " + std::to_string(FLT_MAX) + "), got " + expression->repr())
		)
		std::shared_ptr<daedalus::core::ast::NumberExpression> numberExpression = std::dynamic_pointer_cast<daedalus::core::ast::NumberExpression>(expression);
		DAE_ASSERT_TRUE(
			(numberExpression->value >= FLT_MIN && numberExpression->value <= FLT_MAX),
			std::runtime_error("Expected valid u64 value (" + std::to_string(FLT_MIN) + " - " + std::to_string(FLT_MAX) + "), got " + std::to_string(numberExpression->value))
		)
	}
	else if(type == "f64") {
		DAE_ASSERT_TRUE(
			expression->type() == "NumberExpression",
			std::runtime_error("Expected valid f64 value (" + std::to_string(DBL_MIN) + " - " + std::to_string(DBL_MAX) + "), got " + expression->repr())
		)
		std::shared_ptr<daedalus::core::ast::NumberExpression> numberExpression = std::dynamic_pointer_cast<daedalus::core::ast::NumberExpression>(expression);
		DAE_ASSERT_TRUE(
			(numberExpression->value >= DBL_MIN && numberExpression->value <= DBL_MAX),
			std::runtime_error("Expected valid u64 value (" + std::to_string(DBL_MIN) + " - " + std::to_string(DBL_MAX) + "), got " + std::to_string(numberExpression->value))
		)
	}
	else if(type == "bool") {
		DAE_ASSERT_TRUE(
			expression->type() == "BooleanExpression",
			std::runtime_error("Expected valid boolean value (true / false), got " + expression->repr())
		)
	}
	else if(type == "char") {
		DAE_ASSERT_TRUE(
			expression->type() == "CharExpression",
			std::runtime_error("Expected valid char value, got " + expression->repr())
		)
	}
	else if(type == "str") {
		DAE_ASSERT_TRUE(
			expression->type() == "StrExpression",
			std::runtime_error("Expected valid string value, got " + expression->repr())
		)
	}

	DAE_ASSERT_TRUE(
		eat(tokens).type == "SEMICOLON",
		std::runtime_error("Expected semicolon at the end of line")
	)

	return std::make_shared<daedalus::entropia::ast::AssignationExpression>(identifier, expression);
}

std::shared_ptr<daedalus::core::ast::Statement> daedalus::entropia::parser::parse_declaration_expression(daedalus::core::parser::Parser& parser, std::vector<daedalus::core::lexer::Token>& tokens) {

	if(peek(tokens).type != "LET") {
		return parse_assignation_expression(parser, tokens);
	}
	(void)eat(tokens);

	bool isMutable = false;

	if(peek(tokens).type == "MUT") {
		isMutable = true;
		(void)eat(tokens);
	}

	std::shared_ptr<daedalus::core::ast::Expression> pseudoIdentifier = daedalus::entropia::parser::parse_identifier(parser, tokens);

	DAE_ASSERT_TRUE(
		pseudoIdentifier->type() == "Identifier",
		std::runtime_error("Expected identifier")
	)

	std::shared_ptr<daedalus::entropia::ast::Identifier> identifier = std::dynamic_pointer_cast<daedalus::entropia::ast::Identifier>(pseudoIdentifier);

	(void)expect(tokens, "COLON", std::runtime_error("Expected colon"));

	std::string type = expect(tokens, "TYPE", std::runtime_error("Expected type specifier")).value;

	(void)expect(tokens, "ASSIGN", std::runtime_error("Expected assignment symbol"));

	std::shared_ptr<daedalus::core::ast::Expression> expression = daedalus::entropia::parser::parse_binary_expression(parser, tokens)->get_constexpr();

	if(std::shared_ptr<daedalus::entropia::ast::ContainerExpression> containerExpression = std::dynamic_pointer_cast<daedalus::entropia::ast::ContainerExpression>(expression)) {
		if(std::shared_ptr<daedalus::entropia::ast::Identifier> source_identifier = std::dynamic_pointer_cast<daedalus::entropia::ast::ContainerExpression>(expression)->contains_identifier()) {
			daedalus::entropia::parser::identifiers[identifier->get_name()] = daedalus::entropia::parser::identifiers.at(source_identifier->get_name());
			return std::make_shared<daedalus::entropia::ast::DeclarationExpression>(identifier, expression, type, isMutable);
		}
	}

	if(type == "i8") {
		DAE_ASSERT_TRUE(
			expression->type() == "NumberExpression",
			std::runtime_error("Expected valid i8 value (" + std::to_string(_I8_MIN) + " - " + std::to_string(_I8_MAX) + "), got " + expression->repr())
		)
		std::shared_ptr<daedalus::core::ast::NumberExpression> numberExpression = std::dynamic_pointer_cast<daedalus::core::ast::NumberExpression>(expression);
		DAE_ASSERT_TRUE(
			(numberExpression->value >= _I8_MIN && numberExpression->value <= _I8_MAX),
			std::runtime_error("Expected valid i8 value (" + std::to_string(_I8_MIN) + " - " + std::to_string(_I8_MAX) + "), got " + std::to_string(numberExpression->value))
		)
	}
	else if(type == "i16") {
		DAE_ASSERT_TRUE(
			expression->type() == "NumberExpression",
			std::runtime_error("Expected valid i16 value (" + std::to_string(_I16_MIN) + " - " + std::to_string(_I16_MAX) + "), got " + expression->repr())
		)
		std::shared_ptr<daedalus::core::ast::NumberExpression> numberExpression = std::dynamic_pointer_cast<daedalus::core::ast::NumberExpression>(expression);
		DAE_ASSERT_TRUE(
			(numberExpression->value >= _I16_MIN && numberExpression->value <= _I16_MAX),
			std::runtime_error("Expected valid i16 value (" + std::to_string(_I16_MIN) + " - " + std::to_string(_I16_MAX) + "), got " + std::to_string(numberExpression->value))
		)
	}
	else if(type == "i32") {
		DAE_ASSERT_TRUE(
			expression->type() == "NumberExpression",
			std::runtime_error("Expected valid i32 value (" + std::to_string(_I32_MIN) + " - " + std::to_string(_I32_MAX) + "), got " + expression->repr())
		)
		std::shared_ptr<daedalus::core::ast::NumberExpression> numberExpression = std::dynamic_pointer_cast<daedalus::core::ast::NumberExpression>(expression);
		DAE_ASSERT_TRUE(
			(numberExpression->value >= _I32_MIN && numberExpression->value <= _I32_MAX),
			std::runtime_error("Expected valid i32 value (" + std::to_string(_I32_MIN) + " - " + std::to_string(_I32_MAX) + "), got " + std::to_string(numberExpression->value))
		)
	}
	else if(type == "i64") {
		DAE_ASSERT_TRUE(
			expression->type() == "NumberExpression",
			std::runtime_error("Expected valid i64 value (" + std::to_string(_I64_MIN) + " - " + std::to_string(_I64_MAX) + "), got " + expression->repr())
		)
		std::shared_ptr<daedalus::core::ast::NumberExpression> numberExpression = std::dynamic_pointer_cast<daedalus::core::ast::NumberExpression>(expression);
		DAE_ASSERT_TRUE(
			(numberExpression->value >= _I64_MIN && numberExpression->value <= _I64_MAX),
			std::runtime_error("Expected valid i64 value (" + std::to_string(_I64_MIN) + " - " + std::to_string(_I64_MAX) + "), got " + std::to_string(numberExpression->value))
		)
	}
	else if(type == "u8") {
		DAE_ASSERT_TRUE(
			expression->type() == "NumberExpression",
			std::runtime_error("Expected valid u8 value (0 - " + std::to_string(_UI8_MAX) + "), got " + expression->repr())
		)
		std::shared_ptr<daedalus::core::ast::NumberExpression> numberExpression = std::dynamic_pointer_cast<daedalus::core::ast::NumberExpression>(expression);
		DAE_ASSERT_TRUE(
			(numberExpression->value >= 0 && numberExpression->value <= _UI8_MAX),
			std::runtime_error("Expected valid u8 value (0 - " + std::to_string(_UI8_MAX) + "), got " + std::to_string(numberExpression->value))
		)
	}
	else if(type == "u16") {
		DAE_ASSERT_TRUE(
			expression->type() == "NumberExpression",
			std::runtime_error("Expected valid u16 value (0 - " + std::to_string(_UI16_MAX) + "), got " + expression->repr())
		)
		std::shared_ptr<daedalus::core::ast::NumberExpression> numberExpression = std::dynamic_pointer_cast<daedalus::core::ast::NumberExpression>(expression);
		DAE_ASSERT_TRUE(
			(numberExpression->value >= 0 && numberExpression->value <= _UI16_MAX),
			std::runtime_error("Expected valid u16 value (0 - " + std::to_string(_UI16_MAX) + "), got " + std::to_string(numberExpression->value))
		)
	}
	else if(type == "u32") {
		DAE_ASSERT_TRUE(
			expression->type() == "NumberExpression",
			std::runtime_error("Expected valid u32 value (0 - " + std::to_string(_UI32_MAX) + "), got " + expression->repr())
		)
		std::shared_ptr<daedalus::core::ast::NumberExpression> numberExpression = std::dynamic_pointer_cast<daedalus::core::ast::NumberExpression>(expression);
		DAE_ASSERT_TRUE(
			(numberExpression->value >= 0 && numberExpression->value <= _UI32_MAX),
			std::runtime_error("Expected valid u32 value (0 - " + std::to_string(_UI32_MAX) + "), got " + std::to_string(numberExpression->value))
		)
	}
	else if(type == "u64") {
		DAE_ASSERT_TRUE(
			expression->type() == "NumberExpression",
			std::runtime_error("Expected valid u64 value (0 - " + std::to_string(_UI64_MAX) + "), got " + expression->repr())
		)
		std::shared_ptr<daedalus::core::ast::NumberExpression> numberExpression = std::dynamic_pointer_cast<daedalus::core::ast::NumberExpression>(expression);
		DAE_ASSERT_TRUE(
			(numberExpression->value >= 0 && numberExpression->value <= _UI64_MAX),
			std::runtime_error("Expected valid u64 value (0 - " + std::to_string(_UI64_MAX) + "), got " + std::to_string(numberExpression->value))
		)
	}
	else if(type == "f32") {
		DAE_ASSERT_TRUE(
			expression->type() == "NumberExpression",
			std::runtime_error("Expected valid f32 value (" + std::to_string(FLT_MIN) + " - " + std::to_string(FLT_MAX) + "), got " + expression->repr())
		)
		std::shared_ptr<daedalus::core::ast::NumberExpression> numberExpression = std::dynamic_pointer_cast<daedalus::core::ast::NumberExpression>(expression);
		DAE_ASSERT_TRUE(
			(numberExpression->value >= FLT_MIN && numberExpression->value <= FLT_MAX),
			std::runtime_error("Expected valid u64 value (" + std::to_string(FLT_MIN) + " - " + std::to_string(FLT_MAX) + "), got " + std::to_string(numberExpression->value))
		)
	}
	else if(type == "f64") {
		DAE_ASSERT_TRUE(
			expression->type() == "NumberExpression",
			std::runtime_error("Expected valid f64 value (" + std::to_string(DBL_MIN) + " - " + std::to_string(DBL_MAX) + "), got " + expression->repr())
		)
		std::shared_ptr<daedalus::core::ast::NumberExpression> numberExpression = std::dynamic_pointer_cast<daedalus::core::ast::NumberExpression>(expression);
		DAE_ASSERT_TRUE(
			(numberExpression->value >= DBL_MIN && numberExpression->value <= DBL_MAX),
			std::runtime_error("Expected valid u64 value (" + std::to_string(DBL_MIN) + " - " + std::to_string(DBL_MAX) + "), got " + std::to_string(numberExpression->value))
		)
	}
	else if(type == "bool") {
		DAE_ASSERT_TRUE(
			expression->type() == "BooleanExpression",
			std::runtime_error("Expected valid boolean value (true / false), got " + expression->repr())
		)
	}
	else if(type == "char") {
		DAE_ASSERT_TRUE(
			expression->type() == "CharExpression",
			std::runtime_error("Expected valid char value, got " + expression->repr())
		)
	}
	else if(type == "str") {
		DAE_ASSERT_TRUE(
			expression->type() == "StrExpression",
			std::runtime_error("Expected valid string value, got " + expression->repr())
		)
	}

	DAE_ASSERT_TRUE(
		eat(tokens).type == "SEMICOLON",
		std::runtime_error("Expected semicolon at the end of line")
	)

	daedalus::entropia::parser::identifiers[identifier->get_name()] = type;
	return std::make_shared<daedalus::entropia::ast::DeclarationExpression>(identifier, expression, type, isMutable);
}