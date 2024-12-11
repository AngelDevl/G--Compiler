#pragma once

#include <sstream>
#include <unordered_map>
#include <assert.h>
#include "..//Parsing/Parser.hpp"

class Generator {

public:

    inline explicit Generator(NodeProg prog) 
        : m_prog(std::move(prog)) {}


    void gen_trem(const NodeTerm* term) {
        struct TermVisistor {
            Generator* m_gen;

            void operator ()(const NodeTermIntLit* term_int_lit) const {
                m_gen->m_output << "\tmov rax, " << term_int_lit->int_lit.value.value() << "\n";
                m_gen->push("rax");

            }

            void operator ()(const NodeTermIdent* term_ident) const {
                auto iter = m_gen->m_vars.find(term_ident->ident.value.value());
                if (iter == m_gen->m_vars.end()) {
                    std::cerr << "Undeclared identifier: " << term_ident->ident.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }

                std::pair p = (*iter);
                std::stringstream offset;
                offset << "QWORD [rsp + " <<  (m_gen->m_stack_size - p.second.stacl_loc - 1) * 8<< "]";
                m_gen->push(offset.str());
            }

            void operator ()(const NodeTermParen* term_paren) {
                m_gen->gen_expr(term_paren->expr);
            }
        };

        TermVisistor visitor {this};
        std::visit(visitor, term->var);
    }


    void gen_bin_expr(const NodeBinExpr* expr) {
        struct BinExprVisitor {
            Generator *gen;

            void operator()(const NodeBinExprSub* sub) const {
                gen->gen_expr(sub->rhs);
                gen->gen_expr(sub->lhs);
                gen->pop("rax");
                gen->pop("rbx");
                gen->m_output << "\tsub rax, rbx\n";
                gen->push("rax");
            }

            void operator()(const NodeBinExprDiv* div) const {
                gen->gen_expr(div->rhs);
                gen->gen_expr(div->lhs);
                gen->pop("rax");
                gen->pop("rbx");
                gen->m_output << "\tdiv rbx\n";
                gen->push("rax");
            }

            void operator()(const NodeBinExprAdd* add) const {
                gen->gen_expr(add->rhs);
                gen->gen_expr(add->lhs);
                gen->pop("rax");
                gen->pop("rbx");
                gen->m_output << "\tadd rax, rbx\n";
                gen->push("rax");
            }

            void operator()(const NodeBinExprMulti* multi) const {
                gen->gen_expr(multi->rhs);
                gen->gen_expr(multi->lhs);
                gen->pop("rax");
                gen->pop("rbx");
                gen->m_output << "\tmul rbx\n";
                gen->push("rax");
            }
        };

        BinExprVisitor visitor { this };
        std::visit(visitor, expr->var);
    }


    void gen_expr(const NodeExpr* expr) {
        struct ExprVisitor {
            Generator* m_gen;

            void operator ()(const NodeTerm* term) {
                m_gen->gen_trem(term);
            }

            void operator ()(const NodeBinExpr* node_bin_expr) {
                m_gen->gen_bin_expr(node_bin_expr);
            }

        };


        ExprVisitor visitor {this};
        std::visit(visitor, expr->var);
    }


    void gen_stmt(const NodeStmt* stmt) {
        struct StmtVisitor {
            Generator* m_gen;

            void operator ()(const NodeStmtExit* stmt_exit) const {
                m_gen->gen_expr(stmt_exit->expr);
                m_gen->m_output << "\tmov rax, 60\n";
                m_gen->pop("rdi");
                m_gen->m_output << "\tsyscall\n";
            }

            void operator ()(const NodeStmtLet* stmt_let) {
                if (m_gen->m_vars.contains(stmt_let->ident.value.value())) {
                    std::cerr << "Identifier already used: " << stmt_let->ident.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }

                m_gen->m_vars.insert(std::make_pair(stmt_let->ident.value.value(), Var {m_gen->m_stack_size}));
                m_gen->gen_expr(stmt_let->expr);
            }
        };

        StmtVisitor visitor {this};
        std::visit(visitor, stmt->var);
    }
    
    [[nodiscard]] std::string gen_prog() {
        m_output << "global _start\n_start:\n";

        for (const NodeStmt* stmt: m_prog.stmts) {
            gen_stmt(stmt);
        }

        m_output << "\tmov rax, 60\n";
        m_output << "\tmov rdi, 0" << "\n";
        m_output << "\tsyscall";

        return m_output.str();
    }

private:

    void push(const std::string& reg) {
        m_output << "\tpush " << reg << "\n";
        m_stack_size++;
    }

    void pop(const std::string& reg) {
        m_output << "\tpop " << reg << "\n";
        m_stack_size--;
    }

    struct Var {
        size_t stacl_loc;
    };


    const NodeProg m_prog;
    std::stringstream m_output;
    size_t m_stack_size = 0;
    std::unordered_map<std::string, Var> m_vars;
};