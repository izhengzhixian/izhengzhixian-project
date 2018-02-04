GCC = gcc
MODULES = modules
CALCULATOR = ${MODULES}/calculator
COMMAND = ${MODULES}/command
ANALYZE_UNIT = ${CALCULATOR}/analyze_unit
OPERATOR = ${CALCULATOR}/operator
FUNC = ${CALCULATOR}/func
DEFINE = ${MODULES}/define.h
ALL_LIBS = main.o zcal.o extend.o calculator.o judge.o command.o \
		   analyze_unit.o cal_tree.o func.o generate_tree.o operator.o \
		   analyze_unit_alpha.o analyze_unit_bracket.o \
		   analyze_unit_digit.o analyze_unit_operator.o \
		   _operator_one.o \
		   _operator_two.o _operator_three.o _operator_four.o \
		   _operator_five.o _operator_six.o _operator_seven.o \
		   abs.o exp.o pow.o sqrt.o _handle_digit.o \
		   cal_exit.o help.o
target: zcal_server clean_libs
zcal_server: ${ALL_LIBS} ${DEFINE}
	$(GCC) -o zcal_server ${ALL_LIBS} -lm -lpthread
main.o: main.c ${DEFINE}
	$(GCC) -c main.c
zcal.o: zcal.c ${MODULES}/define.h
	$(GCC) -c zcal.c
extend.o: ${MODULES}/extend.c ${MODULES}/extend.h ${DEFINE}
	$(GCC) -c ${MODULES}/extend.c
calculator.o: ${MODULES}/calculator.c ${MODULES}/calculator.h ${DEFINE}
	$(GCC) -c ${MODULES}/calculator.c
judge.o: ${MODULES}/judge.c ${MODULES}/judge.h ${DEFINE}
	$(GCC) -c ${MODULES}/judge.c
command.o: ${MODULES}/command.c ${MODULES}/command.h ${DEFINE}
	$(GCC) -c ${MODULES}/command.c
analyze_unit.o: ${CALCULATOR}/analyze_unit.c \
	${CALCULATOR}/analyze_unit.h ${DEFINE}
	$(GCC) -c ${CALCULATOR}/analyze_unit.c
cal_tree.o: ${CALCULATOR}/cal_tree.c ${CALCULATOR}/cal_tree.h ${DEFINE}
	$(GCC) -c ${CALCULATOR}/cal_tree.c
func.o: ${CALCULATOR}/func.c ${CALCULATOR}/func.h
	$(GCC) -c ${CALCULATOR}/func.c
generate_tree.o: ${CALCULATOR}/generate_tree.c \
	${CALCULATOR}/generate_tree.h ${DEFINE}
	$(GCC) -c ${CALCULATOR}/generate_tree.c
operator.o: ${CALCULATOR}/operator.c ${CALCULATOR}/operator.h ${DEFINE}
	$(GCC) -c ${CALCULATOR}/operator.c
analyze_unit_alpha.o: ${ANALYZE_UNIT}/analyze_unit_alpha.c \
	${ANALYZE_UNIT}/analyze_unit_alpha.h ${DEFINE}
	$(GCC) -c ${ANALYZE_UNIT}/analyze_unit_alpha.c
analyze_unit_bracket.o: ${ANALYZE_UNIT}/analyze_unit_bracket.c \
	${ANALYZE_UNIT}/analyze_unit_bracket.h ${DEFINE}
	$(GCC) -c ${ANALYZE_UNIT}/analyze_unit_bracket.c
analyze_unit_digit.o: ${ANALYZE_UNIT}/analyze_unit_digit.c \
	${ANALYZE_UNIT}/analyze_unit_digit.h ${DEFINE}
	$(GCC) -c ${ANALYZE_UNIT}/analyze_unit_digit.c
analyze_unit_operator.o: ${ANALYZE_UNIT}/analyze_unit_operator.c \
	${ANALYZE_UNIT}/analyze_unit_operator.h ${DEFINE}
	$(GCC) -c ${ANALYZE_UNIT}/analyze_unit_operator.c
_operator_one.o: ${OPERATOR}/_operator_one.c \
	${OPERATOR}/operator_func.h ${DEFINE}
	$(GCC) -c ${OPERATOR}/_operator_one.c
_operator_two.o: ${OPERATOR}/_operator_two.c \
	${OPERATOR}/operator_func.h ${DEFINE}
	$(GCC) -c ${OPERATOR}/_operator_two.c
_operator_three.o: ${OPERATOR}/_operator_three.c \
	${OPERATOR}/operator_func.h ${DEFINE}
	$(GCC) -c ${OPERATOR}/_operator_three.c
_operator_four.o: ${OPERATOR}/_operator_four.c \
	${OPERATOR}/operator_func.h ${DEFINE}
	$(GCC) -c ${OPERATOR}/_operator_four.c
_operator_five.o: ${OPERATOR}/_operator_five.c \
	${OPERATOR}/operator_func.h ${DEFINE}
	$(GCC) -c ${OPERATOR}/_operator_five.c
_operator_six.o: ${OPERATOR}/_operator_six.c \
	${OPERATOR}/operator_func.h ${DEFINE}
	$(GCC) -c ${OPERATOR}/_operator_six.c
_operator_seven.o: ${OPERATOR}/_operator_seven.c \
	${OPERATOR}/operator_func.h ${DEFINE}
	$(GCC) -c ${OPERATOR}/_operator_seven.c
abs.o: ${FUNC}/abs.c ${FUNC}/func_head.h ${DEFINE}
	$(GCC) -c ${FUNC}/abs.c
exp.o: ${FUNC}/exp.c ${FUNC}/func_head.h ${DEFINE}
	$(GCC) -c ${FUNC}/exp.c
pow.o: ${FUNC}/pow.c ${FUNC}/func_head.h ${DEFINE}
	$(GCC) -c ${FUNC}/pow.c
sqrt.o: ${FUNC}/sqrt.c ${FUNC}/func_head.h ${DEFINE}
	$(GCC) -c ${FUNC}/sqrt.c
_handle_digit.o: ${FUNC}/_handle_digit.c ${FUNC}/_handle_digit.h ${DEFINE}
	$(GCC) -c ${FUNC}/_handle_digit.c
cal_exit.o: ${COMMAND}/cal_exit.c ${COMMAND}/cal_exit.h ${DEFINE}
	$(GCC) -c ${COMMAND}/cal_exit.c
help.o: ${COMMAND}/help.c ${COMMAND}/help.h ${DEFINE}
	$(GCC) -c ${COMMAND}/help.c

.PHONY: clean_lib clean
clean_libs:
	rm -rf *.o
clean:
	rm -rf *.o zcal_server
