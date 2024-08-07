#ifndef __DAEDALUS_ENTROPIA_AST__
#define __DAEDALUS_ENTROPIA_AST__

#include <daedalus/core/parser/ast.hpp>
#include <daedalus/core/tools/assert.hpp>

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace daedalus {
    namespace entropia {
    	namespace ast {

    		/**
    		 * TODO
    		 * FunctionCallExpression
    		 * FunctionDeclarationExpression
    		 * MatchStructure
    		 * TypeDeclaration
    		 * TypeImplementation
    		 */

    		// From daedalus core
    		// class Statement;
    		// class Expression;
    		// class Scope;
    		// class NumberExpression;

    		class Identifier;

    		class AssignationExpression;
    		class DeclarationExpression;

    		class BooleanExpression;
    		class CharExpression;
			class StrExpression;

    		class ContainerExpression;
    		class UnaryExpression;
    		class BinaryExpression;

			class LoopExpression;
			class WhileExpression;
			class ForExpression;

			class BreakExpression;
			class ContinueExpression;

			class OrExpression;

			class ConditionnalExpression;
			class ConditionnalStructure;

    		class Identifier : public daedalus::core::ast::Expression {
    		public:
    			Identifier(std::string name);

    			std::string get_name();

    			virtual std::string type() override;
    			virtual std::shared_ptr<daedalus::core::ast::Expression> get_constexpr() override;
    			virtual std::string repr(int indent = 0) override;

    		protected:
    			std::string name;
    		};

    		class AssignationExpression : public daedalus::core::ast::Expression {
    		public:
    			AssignationExpression(
    				std::shared_ptr<Identifier> identifier,
    				std::shared_ptr<daedalus::core::ast::Expression> value
    			);

    			std::shared_ptr<Identifier> get_identifier();
    			std::shared_ptr<daedalus::core::ast::Expression> get_value();

    			virtual std::string type() override;
    			virtual std::shared_ptr<daedalus::core::ast::Expression> get_constexpr() override;
    			virtual std::string repr(int indent = 0) override;

    		protected:
    			std::shared_ptr<Identifier> identifier;
    			std::shared_ptr<daedalus::core::ast::Expression> value;
    		};

    		class DeclarationExpression : public AssignationExpression {
    		public:
    			DeclarationExpression(std::shared_ptr<Identifier> identifier, std::shared_ptr<daedalus::core::ast::Expression> value, std::string value_type, bool isMutable);

    			std::string get_value_type();
    			bool get_mutability();

    			virtual std::string type() override;
    			virtual std::shared_ptr<daedalus::core::ast::Expression> get_constexpr() override;
    			virtual std::string repr(int indent = 0) override;

    		protected:
    			std::string value_type;
    			bool isMutable;
    		};

    		class BooleanExpression : public daedalus::core::ast::Expression {
    		public:
     			BooleanExpression(bool value);

                bool get_value();
                void set_value(bool value);

    			virtual std::string type() override;
    			virtual std::shared_ptr<daedalus::core::ast::Expression> get_constexpr() override;
    			virtual std::string repr(int indent = 0) override;

            protected:
                bool value;
    		};

    		class CharExpression : public daedalus::core::ast::Expression {
    		public:
    			CharExpression(char value);

                char get_value();

    			virtual std::string type() override;
    			virtual std::shared_ptr<daedalus::core::ast::Expression> get_constexpr() override;
    			virtual std::string repr(int indent = 0) override;

            protected:
                char value;
    		};

    		class StrExpression : public daedalus::core::ast::Expression {
    		public:

    			StrExpression(std::string value);

                std::string get_value();

    			virtual std::string type() override;
    			virtual std::shared_ptr<daedalus::core::ast::Expression> get_constexpr() override;
    			virtual std::string repr(int indent = 0) override;

            protected:
    			std::string value;
    		};

    		class ContainerExpression : public daedalus::core::ast::Expression {
    		public:
    			virtual std::shared_ptr<Identifier> get_inner_identifier();
    		};

    		class UnaryExpression : public ContainerExpression {
    		public:
    			UnaryExpression(
    				std::shared_ptr<Expression> term,
    				std::string operator_symbol
    			);

    			std::shared_ptr<Expression> get_term();
    			std::string get_operator_symbol();

    			virtual std::shared_ptr<Identifier> get_inner_identifier() override;

    			virtual std::string type() override;
    			virtual std::shared_ptr<daedalus::core::ast::Expression> get_constexpr() override;
    			virtual std::string repr(int indent = 0) override;

    		protected:
    			std::shared_ptr<Expression> term;
    			std::string operator_symbol;
    		};

    		class BinaryExpression : public ContainerExpression {
    		public:
    			BinaryExpression(
    				std::shared_ptr<Expression> left,
    				std::string operator_symbol,
    				std::shared_ptr<Expression> right
    			);

    			std::shared_ptr<Expression> get_left();
    			std::string get_operator_symbol();
    			std::shared_ptr<Expression> get_right();

    			virtual std::shared_ptr<Identifier> get_inner_identifier() override;

    			virtual std::string type() override;
    			virtual std::shared_ptr<daedalus::core::ast::Expression> get_constexpr() override;
    			virtual std::string repr(int indent = 0) override;

    		protected:
    			std::shared_ptr<Expression> left;
    			std::string operator_symbol;
    			std::shared_ptr<Expression> right;

    		private:
    			std::shared_ptr<Identifier> left_contains_identifier();
    			std::shared_ptr<Identifier> right_contains_identifier();
    		};

            class OrExpression : public daedalus::core::ast::Expression {
            public:
                OrExpression(std::shared_ptr<daedalus::core::ast::Expression> value);

                std::shared_ptr<daedalus::core::ast::Expression> get_value();

                virtual std::string type() override;
          		virtual std::shared_ptr<daedalus::core::ast::Expression> get_constexpr() override;
          		virtual std::string repr(int indent = 0) override;

            protected:
                std::shared_ptr<daedalus::core::ast::Expression> value;
            };

            class LoopExpression : public daedalus::core::ast::Scope {
            public:
                LoopExpression(
    				std::vector<std::shared_ptr<Expression>> body,
                    std::shared_ptr<OrExpression> orExpression = nullptr
     			);

                std::shared_ptr<OrExpression> get_or_expression();

     			virtual std::string type() override;
     			virtual std::string repr(int indent = 0) override;

            protected:
                std::shared_ptr<OrExpression> orExpression;
            };

            class WhileExpression : public LoopExpression {
            public:
                WhileExpression(
                    std::vector<std::shared_ptr<Expression>> body,
                    std::shared_ptr<Expression> condition,
                    std::shared_ptr<OrExpression> orExpression = nullptr
                );

                std::shared_ptr<Expression> get_condition();

                virtual std::string type() override;
     			virtual std::string repr(int indent = 0) override;

            protected:
                std::shared_ptr<Expression> condition;
            };

            class ForExpression : public WhileExpression {
            public:
                ForExpression(
                    std::vector<std::shared_ptr<Expression>> body,
                    std::shared_ptr<Expression> initial_expression,
                    std::shared_ptr<Expression> condition,
                    std::shared_ptr<Expression> update_expression,
                    std::shared_ptr<OrExpression> orExpression = nullptr
                );

                std::shared_ptr<Expression> get_initial_expression();
                std::shared_ptr<Expression> get_update_expression();

                virtual std::string type() override;
     			virtual std::string repr(int indent = 0) override;

            protected:
                std::shared_ptr<Expression> initialExpression;
                std::shared_ptr<Expression> updateExpression;
            };

            class BreakExpression : public daedalus::core::ast::Expression {
            public:
                BreakExpression();

                virtual std::string type() override;
     			virtual std::shared_ptr<daedalus::core::ast::Expression> get_constexpr() override;
     			virtual std::string repr(int indent = 0) override;
            };

            class ContinueExpression : public daedalus::core::ast::Expression {
            public:
                ContinueExpression();

                virtual std::string type() override;
     			virtual std::shared_ptr<daedalus::core::ast::Expression> get_constexpr() override;
     			virtual std::string repr(int indent = 0) override;
            };

            class ConditionnalExpression : public daedalus::core::ast::Scope {
            public:
                ConditionnalExpression(
                    std::vector<std::shared_ptr<Expression>> body,
                    std::shared_ptr<daedalus::core::ast::Expression> condition,
                    std::shared_ptr<daedalus::entropia::ast::ConditionnalExpression> before = nullptr
                );

                std::shared_ptr<ConditionnalExpression> get_before();
                std::shared_ptr<daedalus::core::ast::Expression> get_condition();

     			virtual std::string type() override;
     			virtual std::string repr(int indent = 0) override;

      		protected:
                std::shared_ptr<ConditionnalExpression> before;
     			std::shared_ptr<daedalus::core::ast::Expression> condition;
            };

            class ConditionnalStructure : public daedalus::core::ast::Expression {
            public:
                ConditionnalStructure(
                    std::vector<std::shared_ptr<ConditionnalExpression>> expressions
                );

                std::vector<std::shared_ptr<ConditionnalExpression>> get_expressions();

                virtual std::string type() override;
     			virtual std::shared_ptr<daedalus::core::ast::Expression> get_constexpr() override;
     			virtual std::string repr(int indent = 0) override;

            protected:
                std::vector<std::shared_ptr<ConditionnalExpression>> expressions;
            };
    	}
    }
}

#endif // __DAEDALUS_ENTROPIA_AST__
