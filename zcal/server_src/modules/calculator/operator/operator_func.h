#ifndef _OPERATOR_FUNC_H
#define _OPERATOR_FUNC_H

/* level_one */
Result _operator_pow(Result left, Result right);

/* level_two */
Result _operator_not(Result left, Result right);

/* level_three */
Result _operator_multiply(Result left, Result right);
Result _operator_divide(Result left, Result right);
Result _operator_remainder(Result left, Result right);
Result _operator_exact_divide(Result left, Result right);

/* level_four */
Result _operator_add(Result left, Result right);
Result _operator_sub(Result left, Result right);

/* level_five */
Result _operator_and(Result left, Result right);

/* level_six */
Result _operator_xor(Result left, Result right);

/* level_seven*/
Result _operator_or(Result left, Result right);




#endif
