/* unparseJovial_statements.C
 * Contains functions that unparse statements
 *
 * FORMATTING WILL BE DONE IN TWO WAYS:
 * 1. using the file_info object to get information from line and column number 
 *    (for original source code)
 * 2. following a specified format that I have specified with indentations of
 *    length TABINDENT (for transformations)
 * 
 * REMEMBER: For types and symbols, we still call the original unparse function 
 * defined in sage since they dont have file_info. For expressions, 
 * Unparse_Jovial::unparse is called, and for statements, 
 * Unparse_Jovial::unparseStatement is called.
 *
 */
#include "sage3basic.h"
#include "unparser.h"

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#define ROSE_TRACK_PROGRESS_OF_ROSE_COMPILING_ROSE 0

using namespace std;
using std::cerr;
using std::endl;

#include "sage_support.h"


Unparse_Jovial::Unparse_Jovial(Unparser* unp, std::string fname)
   : UnparseLanguageIndependentConstructs(unp,fname)
   {
   }


Unparse_Jovial::~Unparse_Jovial()
   {
   }


void 
Unparse_Jovial::unparseJovialFile(SgSourceFile *sourcefile, SgUnparse_Info& info) 
   {
     SgGlobal* globalScope = sourcefile->get_globalScope();
     ROSE_ASSERT(globalScope != NULL);

     curprint("START\n");
     unparseStatement(globalScope, info);
     curprint("TERM\n");
   }


void
Unparse_Jovial::unparseLanguageSpecificStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
  // This function unparses the language specific statements not handled by the base class unparseStatement() member function

     ROSE_ASSERT(stmt != NULL);

  // curprint_indented("", info);

     switch (stmt->variantT())
        {
       // case V_SgGlobal:                   cout << "Got it !!!" << endl; /* unparseGlobalStmt   (stmt, info); */ break;


       // program units
       // case V_SgModuleStatement:            unparseModuleStmt(stmt, info);       break;
          case V_SgProgramHeaderStatement:     unparseProgHdrStmt(stmt, info);      break;
       // case V_SgProcedureHeaderStatement:   unparseProcHdrStmt(stmt, info);      break;

       // declarations
       // case V_SgFunctionDeclaration:        unparseFuncDeclStmt(stmt, info);     break;
          case V_SgFunctionDefinition:         unparseFuncDefnStmt(stmt, info);     break;

          case V_SgVariableDeclaration:        unparseVarDeclStmt (stmt, info);     break;

       // executable statements, control flow
          case V_SgBasicBlock:                 unparseBasicBlockStmt (stmt, info);  break;
          case V_SgLabelStatement:             unparseLabelStmt      (stmt, info);  break;
          case V_SgGotoStatement:              unparseGotoStmt       (stmt, info);  break;
          case V_SgIfStmt:                     unparseIfStmt         (stmt, info);  break;

          case V_SgStopOrPauseStatement:       unparseStopOrPauseStmt(stmt, info);  break;
          case V_SgReturnStmt:                 unparseReturnStmt     (stmt, info);  break;

          case V_SgExprStatement:              unparseExprStmt       (stmt, info);  break;

#if 0
       // declarations
          case V_SgVariableDefinition:     unparseVarDefnStmt  (stmt, info); break;

       // executable statements, control flow

          case V_SgWhileStmt:              unparseWhileStmt      (stmt, info); break;
          case V_SgSwitchStatement:        unparseSwitchStmt     (stmt, info); break;
          case V_SgCaseOptionStmt:         unparseCaseStmt       (stmt, info); break;
          case V_SgDefaultOptionStmt:      unparseDefaultStmt    (stmt, info); break;
          case V_SgBreakStmt:              unparseBreakStmt      (stmt, info); break;
          case V_SgAssertStmt:             unparseAssertStmt     (stmt, info); break;

          case V_SgForStatement:           unparseForStmt(stmt, info);          break; 

          case V_SgEnumDeclaration:        unparseEnumDeclStmt(stmt, info);     break;

          case V_SgDoWhileStmt:            unparseDoWhileStmt(stmt, info);      break;

          case V_SgContinueStmt:           unparseContinueStmt(stmt, info);     break;

          case V_SgTypedefDeclaration:     unparseTypeDefStmt(stmt, info);      break;

          case V_SgForInitStatement:                   unparseForInitStmt(stmt, info); break;

          case V_SgFunctionParameterList:  unparseFunctionParameterList(stmt, info); break;

          case V_SgUsingDirectiveStatement:            unparseUsingDirectiveStatement (stmt, info);            break;
          case V_SgUsingDeclarationStatement:          unparseUsingDeclarationStatement (stmt, info);          break;
#endif

          default:
             {
                cerr << "Unparse_Jovial::unparseLanguageSpecificStatement: Error: No handler for "
                     <<  stmt->class_name() << ", variant: " << stmt->variantT() << endl;
               ROSE_ASSERT(false);
               break;
             }
        }
   }


//----------------------------------------------------------------------------
//  Unparse_Jovial::MODULES
//----------------------------------------------------------------------------

void 
Unparse_Jovial::unparseProgHdrStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgUnparse_Info ninfo(info);

     SgProgramHeaderStatement* prog = isSgProgramHeaderStatement(stmt);
     ROSE_ASSERT(prog);

     curprint("PROGRAM ");
     curprint(prog->get_name().str());
     curprint(" ;\n");

     unparseStatement(prog->get_definition(), ninfo);

 //  unparseStatementNumbersSupport(mod->get_end_numeric_label(),info);

  // TODO - unparse non-nested-subroutines
   }

void
Unparse_Jovial::unparseFuncDefnStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgFunctionDefinition* funcdef = isSgFunctionDefinition(stmt);
     ROSE_ASSERT(funcdef != NULL);

     curprint("BEGIN\n");

  // unparse the body of the function
     if (funcdef->get_body())
        {
          unparseStatement(funcdef->get_body(), info);
        }

     curprint("END\n");
   }


//----------------------------------------------------------------------------
//  Unparse_Jovial::<executable statements, control flow>
//----------------------------------------------------------------------------

void
Unparse_Jovial::unparseBasicBlockStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgBasicBlock* basic_stmt = isSgBasicBlock(stmt);
     ROSE_ASSERT(basic_stmt != NULL);

#if 0
  // DQ (10/6/2008): Adding space here is required to get "else if" blocks formatted correctly (at least).
     unp->cur.format(basic_stmt, info, FORMAT_BEFORE_BASIC_BLOCK1);
#endif

     SgStatementPtrList::iterator p = basic_stmt->get_statements().begin();
     for ( ; p != basic_stmt->get_statements().end(); ++p)
     {
          unparseStatement((*p), info);
     }

#if 0
  // DQ (10/6/2008): This does not appear to be required (passes all tests).
     unp->cur.format(basic_stmt, info, FORMAT_AFTER_BASIC_BLOCK1);
#endif
   }

void Unparse_Jovial::unparseLabelStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgLabelStatement* label_stmt = isSgLabelStatement(stmt);
     ROSE_ASSERT(label_stmt != NULL);

     curprint (string(label_stmt->get_label().str()) + ":");
     unp->cur.insert_newline(1);

     if (label_stmt->get_statement() != NULL) {
        SgStatement* sg_stmt = label_stmt->get_statement();
        ROSE_ASSERT(sg_stmt);
        UnparseLanguageIndependentConstructs::unparseStatement(sg_stmt, info);
     }
   }

void
Unparse_Jovial::unparseGotoStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgGotoStatement* goto_stmt = isSgGotoStatement(stmt);
     ROSE_ASSERT(goto_stmt != NULL);
     ROSE_ASSERT(goto_stmt->get_label() != NULL);

     curprint (string("GOTO " ) + goto_stmt->get_label()->get_label().str());
     curprint (string(" ;"));
     unp->cur.insert_newline(1);
   }

void
Unparse_Jovial::unparseIfStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgIfStmt* if_stmt = isSgIfStmt(stmt);
     ROSE_ASSERT(if_stmt != NULL);
     ROSE_ASSERT(if_stmt->get_conditional());

  // condition
     curprint("IF (");
     info.set_inConditional();

     SgExprStatement* expressionStatement = isSgExprStatement(if_stmt->get_conditional());
     unparseExpression(expressionStatement->get_expression(), info);

     info.unset_inConditional();
     curprint(") ;");
     unp->cur.insert_newline(1);

  // true body
     ROSE_ASSERT(if_stmt->get_true_body());
     unparseStatement(if_stmt->get_true_body(), info);

  // false body
     if (if_stmt->get_false_body() != NULL) {
        curprint("ELSE");
        unp->cur.insert_newline(1);
        unparseStatement(if_stmt->get_false_body(), info);
     }
   }

void
Unparse_Jovial::unparseStopOrPauseStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgStopOrPauseStatement* sp_stmt = isSgStopOrPauseStatement(stmt);
     ROSE_ASSERT(sp_stmt != NULL);

     SgStopOrPauseStatement::stop_or_pause_enum kind = sp_stmt->get_stop_or_pause();

     if (kind == SgStopOrPauseStatement::e_stop)
        {
          curprint("STOP ");
          unparseExpression(sp_stmt->get_code(), info);
          curprint(";");
          unp->cur.insert_newline(1);
        }
     else if (kind == SgStopOrPauseStatement::e_exit)
        {
          curprint("EXIT ;");
          unp->cur.insert_newline(1);
        }
     else if (kind == SgStopOrPauseStatement::e_abort)
        {
          curprint("ABORT ;");
          unp->cur.insert_newline(1);
        }
     else
        {
          cerr << "Unparse_Jovial::unparseStopOrPauseStmt: unknown statement enum "
               <<  kind << endl;
          ROSE_ASSERT(false);
        }
   }

void
Unparse_Jovial::unparseReturnStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
      SgReturnStmt* return_stmt = isSgReturnStmt(stmt);
      ROSE_ASSERT(return_stmt != NULL);

      curprint("RETURN ;");
      unp->cur.insert_newline(1);
   }

void
Unparse_Jovial::unparseVarDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgVariableDeclaration* vardecl = isSgVariableDeclaration(stmt);
     ROSE_ASSERT(vardecl != NULL);
  
     SgInitializedNamePtrList::iterator p = vardecl->get_variables().begin();

  // Jovial has only one variable per declaration
     unparseVarDecl(vardecl, *p, info);
   }

void
Unparse_Jovial::unparseVarDecl(SgStatement* stmt, SgInitializedName* initializedName, SgUnparse_Info& info)
   {
     SgName name         = initializedName->get_name();
     SgType* type        = initializedName->get_type();
     SgInitializer* init = initializedName->get_initializer();  
     ROSE_ASSERT(type);

     SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(stmt);
     ROSE_ASSERT(variableDeclaration != NULL);

#if 0
     if (variableDeclaration->get_declarationModifier().get_typeModifier().isStatic() == true)
        {
           curprint("STATIC ");
        }
#endif

     switch (type->variantT())
        {
          case V_SgArrayType:
             curprint("TABLE ");
             curprint(name.str());
             break;
          default:
             curprint("ITEM ");
             curprint(name.str());
             curprint(" ");
        }

     unparseType(type, info);

     if (init != NULL)
        {
           curprint(" = ");
           SgInitializer* initializer = isSgInitializer(init);
           ROSE_ASSERT(initializer != NULL);
           // TODO
           // unparseExpression(initializer, info);
        }

     curprint(" ;\n");
   }

void
Unparse_Jovial::unparseExprStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgExprStatement* expr_stmt = isSgExprStatement(stmt);
     ROSE_ASSERT(expr_stmt != NULL);
     ROSE_ASSERT(expr_stmt->get_expression());

     unparseExpression(expr_stmt->get_expression(), info);

     unp->u_sage->curprint_newline();
   }

