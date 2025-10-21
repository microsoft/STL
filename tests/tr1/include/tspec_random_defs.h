// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// common header for random distribution tests

typedef unsigned int bernoulli_distribution_type;
bernoulli_distribution bernoulli_distribution_dist(0.1);
int bernoulli_distribution_smaller_ok = 0;
int bernoulli_distribution_larger_ok  = 1;

One_arg bernoulli_distribution_vals[] = {
    {FLIT(0.0), FLIT(0.9000000000000000221), FLIT(0.0)},
    {FLIT(11.0), FLIT(0.0), FLIT(0.0)},
    {FLIT(22.0), FLIT(0.0), FLIT(0.0)},
    {FLIT(33.0), FLIT(0.0), FLIT(0.0)},
    {FLIT(44.0), FLIT(0.0), FLIT(0.0)},
    {FLIT(55.0), FLIT(0.0), FLIT(0.0)},
    {FLIT(66.0), FLIT(0.0), FLIT(0.0)},
    {FLIT(77.0), FLIT(0.0), FLIT(0.0)},
    {FLIT(88.0), FLIT(0.0), FLIT(0.0)},
    {FLIT(99.0), FLIT(0.0), FLIT(0.0)},
};

typedef unsigned int binomial_distribution_type;
binomial_distribution<binomial_distribution_type> binomial_distribution_dist(500, 0.14);
int binomial_distribution_smaller_ok = 0;
int binomial_distribution_larger_ok  = 1;

One_arg binomial_distribution_vals[] = {
    {FLIT(0.0), FLIT(1.7751114354849962758e-33), FLIT(0.0)},
    {FLIT(11.0), FLIT(4.1357571813233832335e-20), FLIT(0.0)},
    {FLIT(22.0), FLIT(1.0667596861038720127e-12), FLIT(0.0)},
    {FLIT(33.0), FLIT(7.7882289340192345399e-08), FLIT(0.0)},
    {FLIT(44.0), FLIT(0.0001107644245134541864), FLIT(0.0)},
    {FLIT(55.0), FLIT(0.0077458464221360464121), FLIT(0.0)},
    {FLIT(66.0), FLIT(0.045989309489316600178), FLIT(0.0)},
    {FLIT(77.0), FLIT(0.033223156045591555051), FLIT(0.0)},
    {FLIT(88.0), FLIT(0.0037651429261223163908), FLIT(0.0)},
    {FLIT(99.0), FLIT(8.0798037229550803347e-05), FLIT(0.0)},
};

typedef double exponential_distribution_type;
exponential_distribution<exponential_distribution_type> exponential_distribution_dist(0.9);
int exponential_distribution_smaller_ok = 0;
int exponential_distribution_larger_ok  = 1;

One_arg exponential_distribution_vals[] = {
    {FLIT(0.0), FLIT(0.59343034025940089028), FLIT(0.0)},
    {FLIT(11.0), FLIT(2.9775178645003367278e-05), FLIT(0.0)},
    {FLIT(22.0), FLIT(1.4939601216788643781e-09), FLIT(0.0)},
    {FLIT(33.0), FLIT(7.4958974109841988164e-14), FLIT(0.0)},
    {FLIT(44.0), FLIT(3.7610426932183981179e-18), FLIT(0.0)},
    {FLIT(55.0), FLIT(1.8870912133193438463e-22), FLIT(0.0)},
    {FLIT(66.0), FLIT(9.4684201639300156424e-27), FLIT(0.0)},
    {FLIT(77.0), FLIT(4.7507497129946777994e-31), FLIT(0.0)},
    {FLIT(88.0), FLIT(2.3836735637797403083e-35), FLIT(0.0)},
    {FLIT(99.0), FLIT(1.1960006318835876534e-39), FLIT(0.0)},
};

typedef double gamma_distribution_type;
gamma_distribution<gamma_distribution_type> gamma_distribution_dist(33.33);
int gamma_distribution_smaller_ok = 0;
int gamma_distribution_larger_ok  = 1;

One_arg gamma_distribution_vals[] = {
    {FLIT(0.0), FLIT(1.3673309796216089079e-38), FLIT(0.0)},
    {FLIT(11.0), FLIT(2.6988940302906503063e-07), FLIT(17.0)},
    {FLIT(22.0), FLIT(0.010638974806815103269), FLIT(10.0)},
    {FLIT(33.0), FLIT(0.068471582683104090239), FLIT(1.0)},
    {FLIT(44.0), FLIT(0.011132866486719340122), FLIT(13.0)},
    {FLIT(55.0), FLIT(0.00023592964425101634248), FLIT(26.0)},
    {FLIT(66.0), FLIT(1.3678876793096542283e-06), FLIT(47.0)},
    {FLIT(77.0), FLIT(3.2322794791835488387e-09), FLIT(65.0)},
    {FLIT(88.0), FLIT(3.9534813028245743808e-12), FLIT(44.0)},
    {FLIT(99.0), FLIT(2.9218019755081229669e-15), FLIT(72.0)},
};

typedef unsigned int geometric_distribution_type;
geometric_distribution<geometric_distribution_type> geometric_distribution_dist(0.0005);
int geometric_distribution_smaller_ok = 0;
int geometric_distribution_larger_ok  = 1;

One_arg geometric_distribution_vals[] = {
    {FLIT(0.0), FLIT(0.00050000000000000001042), FLIT(0.0)},
    {FLIT(11.0), FLIT(0.00049725686469780528559), FLIT(0.0)},
    {FLIT(22.0), FLIT(0.00049452877897818289451), FLIT(0.0)},
    {FLIT(33.0), FLIT(0.00049181566027505028547), FLIT(0.0)},
    {FLIT(44.0), FLIT(0.00048911742647530489972), FLIT(0.0)},
    {FLIT(55.0), FLIT(0.00048643399591633880052), FLIT(0.0)},
    {FLIT(66.0), FLIT(0.0004837652873835672886), FLIT(0.0)},
    {FLIT(77.0), FLIT(0.0004811112201079708531), FLIT(0.0)},
    {FLIT(88.0), FLIT(0.00047847171376365051293), FLIT(0.0)},
    {FLIT(99.0), FLIT(0.00047584668846539719722), FLIT(0.0)},
};

typedef double normal_distribution_type;
normal_distribution<normal_distribution_type> normal_distribution_dist(70.0, 10.0);
int normal_distribution_smaller_ok = 1;
int normal_distribution_larger_ok  = 1;

One_arg normal_distribution_vals[] = {
    {FLIT(0.0), FLIT(1.3203144217523378428e-12), FLIT(0.0)},
    {FLIT(11.0), FLIT(1.4982381152267289312e-09), FLIT(0.0)},
    {FLIT(22.0), FLIT(5.0747930194168739104e-07), FLIT(0.0)},
    {FLIT(33.0), FLIT(5.1308856680145541142e-05), FLIT(0.0)},
    {FLIT(44.0), FLIT(0.0015484773020573848902), FLIT(0.0)},
    {FLIT(55.0), FLIT(0.013949457964912981289), FLIT(0.0)},
    {FLIT(66.0), FLIT(0.037510319421371528247), FLIT(0.0)},
    {FLIT(77.0), FLIT(0.030108253639676330755), FLIT(0.0)},
    {FLIT(88.0), FLIT(0.0072137592969240046241), FLIT(0.0)},
    {FLIT(99.0), FLIT(0.00051591526875394339882), FLIT(0.0)},
};

typedef unsigned int poisson_distribution_type;
poisson_distribution<poisson_distribution_type> poisson_distribution_dist(99.99);
int poisson_distribution_smaller_ok = 0;
int poisson_distribution_larger_ok  = 1;

One_arg poisson_distribution_vals[] = {
    {FLIT(0.0), FLIT(3.7574633611456447220e-44), FLIT(0.0)},
    {FLIT(11.0), FLIT(9.4028885517711230718e-30), FLIT(0.0)},
    {FLIT(22.0), FLIT(3.3355900250355996632e-21), FLIT(0.0)},
    {FLIT(33.0), FLIT(4.3129639100861158652e-15), FLIT(0.0)},
    {FLIT(44.0), FLIT(1.4072926384674739807e-10), FLIT(0.0)},
    {FLIT(55.0), FLIT(2.9432374732049666489e-07), FLIT(0.0)},
    {FLIT(66.0), FLIT(6.8573146564132530047e-05), FLIT(0.0)},
    {FLIT(77.0), FLIT(0.0025682333322709681791), FLIT(0.0)},
    {FLIT(88.0), FLIT(0.020080267686946835088), FLIT(0.0)},
    {FLIT(99.0), FLIT(0.03986496337366227799), FLIT(0.0)},
};

typedef unsigned int uniform_int_distribution_type;
uniform_int_distribution<uniform_int_distribution_type> uniform_int_distribution_dist(10, 90);
int uniform_int_distribution_smaller_ok = 0;
int uniform_int_distribution_larger_ok  = 0;

One_arg uniform_int_distribution_vals[] = {
    {FLIT(0.0), FLIT(0.0), FLIT(0.0)},
    {FLIT(11.0), FLIT(0.012345679012345678328), FLIT(0.0)},
    {FLIT(22.0), FLIT(0.012345679012345678328), FLIT(0.0)},
    {FLIT(33.0), FLIT(0.012345679012345678328), FLIT(0.0)},
    {FLIT(44.0), FLIT(0.012345679012345678328), FLIT(0.0)},
    {FLIT(55.0), FLIT(0.012345679012345678328), FLIT(0.0)},
    {FLIT(66.0), FLIT(0.012345679012345678328), FLIT(0.0)},
    {FLIT(77.0), FLIT(0.012345679012345678328), FLIT(0.0)},
    {FLIT(88.0), FLIT(0.012345679012345678328), FLIT(0.0)},
    {FLIT(99.0), FLIT(0.0), FLIT(0.0)},
};

typedef double uniform_real_distribution_type;
uniform_real_distribution<uniform_real_distribution_type> uniform_real_distribution_dist(40.2, 60.5);
int uniform_real_distribution_smaller_ok = 0;
int uniform_real_distribution_larger_ok  = 0;

One_arg uniform_real_distribution_vals[] = {
    {FLIT(0.0), FLIT(0.0), FLIT(0.0)},
    {FLIT(11.0), FLIT(0.0), FLIT(0.0)},
    {FLIT(22.0), FLIT(0.0), FLIT(0.0)},
    {FLIT(33.0), FLIT(0.0), FLIT(0.0)},
    {FLIT(44.0), FLIT(0.049261083743842366821), FLIT(0.0)},
    {FLIT(55.0), FLIT(0.049261083743842366821), FLIT(0.0)},
    {FLIT(66.0), FLIT(0.0), FLIT(0.0)},
    {FLIT(77.0), FLIT(0.0), FLIT(0.0)},
    {FLIT(88.0), FLIT(0.0), FLIT(0.0)},
    {FLIT(99.0), FLIT(0.0), FLIT(0.0)},
};

typedef double cauchy_distribution_type;
cauchy_distribution<cauchy_distribution_type> cauchy_distribution_dist(50.0, 20.0);
int cauchy_distribution_smaller_ok = 1;
int cauchy_distribution_larger_ok  = 1;

One_arg cauchy_distribution_vals[] = {
    {FLIT(0.0), FLIT(0.0022337169205865663528), FLIT(0.0)},
    {FLIT(11.0), FLIT(0.0033825494626492065912), FLIT(0.0)},
    {FLIT(22.0), FLIT(0.0055065420833932529275), FLIT(0.0)},
    {FLIT(33.0), FLIT(0.0094707128833737101724), FLIT(0.0)},
    {FLIT(44.0), FLIT(0.014794448700373018234), FLIT(0.0)},
    {FLIT(55.0), FLIT(0.014794448700373018234), FLIT(0.0)},
    {FLIT(66.0), FLIT(0.0094707128833737101724), FLIT(0.0)},
    {FLIT(77.0), FLIT(0.0055065420833932529275), FLIT(0.0)},
    {FLIT(88.0), FLIT(0.0033825494626492065912), FLIT(0.0)},
    {FLIT(99.0), FLIT(0.0022337169205865663528), FLIT(0.0)},
};

typedef double chi_squared_distribution_type;
chi_squared_distribution<chi_squared_distribution_type> chi_squared_distribution_dist(25);
int chi_squared_distribution_smaller_ok = 0;
int chi_squared_distribution_larger_ok  = 1;

One_arg chi_squared_distribution_vals[] = {
    {FLIT(0.0), FLIT(6.3560983166287375127e-14), FLIT(0.0)},
    {FLIT(11.0), FLIT(0.0063777771780991199258), FLIT(0.0)},
    {FLIT(22.0), FLIT(0.058180658367338024261), FLIT(0.0)},
    {FLIT(33.0), FLIT(0.023160221299317922812), FLIT(0.0)},
    {FLIT(44.0), FLIT(0.0024827864750023610763), FLIT(0.0)},
    {FLIT(55.0), FLIT(0.00012884902663430578971), FLIT(0.0)},
    {FLIT(66.0), FLIT(4.2164357002313538093e-06), FLIT(0.0)},
    {FLIT(77.0), FLIT(1.0026741000440351133e-07), FLIT(0.0)},
    {FLIT(88.0), FLIT(1.8865426080766510405e-09), FLIT(0.0)},
    {FLIT(99.0), FLIT(2.9673311806562293539e-11), FLIT(0.0)},
};

typedef double extreme_value_distribution_type;
extreme_value_distribution<extreme_value_distribution_type> extreme_value_distribution_dist(100.0, 50.0);
int extreme_value_distribution_smaller_ok = 1;
int extreme_value_distribution_larger_ok  = 1;

One_arg extreme_value_distribution_vals[] = {
    {FLIT(0.0), FLIT(9.7367891314001451391e-05), FLIT(0.0)},
    {FLIT(11.0), FLIT(0.00033126932474906436652), FLIT(0.0)},
    {FLIT(22.0), FLIT(0.00084739624916670380599), FLIT(0.0)},
    {FLIT(33.0), FLIT(0.0017241855032130344335), FLIT(0.0)},
    {FLIT(44.0), FLIT(0.002919441358705740507), FLIT(0.0)},
    {FLIT(55.0), FLIT(0.0042656576399787964755), FLIT(0.0)},
    {FLIT(66.0), FLIT(0.0055371368821154568625), FLIT(0.0)},
    {FLIT(77.0), FLIT(0.0065364835202489889043), FLIT(0.0)},
    {FLIT(88.0), FLIT(0.007150000321596400658), FLIT(0.0)},
    {FLIT(99.0), FLIT(0.0073570958848930461184), FLIT(0.0)},
};

typedef double fisher_f_distribution_type;
fisher_f_distribution<fisher_f_distribution_type> fisher_f_distribution_dist(100, 100);
int fisher_f_distribution_smaller_ok = 0;
int fisher_f_distribution_larger_ok  = 1;

One_arg fisher_f_distribution_vals[] = {
    {FLIT(0.0), FLIT(0.5), FLIT(0.0)},
    {FLIT(11.0), FLIT(8.3260041343146971517e-28), FLIT(0.0)},
    {FLIT(22.0), FLIT(4.2575806932815351998e-41), FLIT(0.0)},
    {FLIT(33.0), FLIT(2.4240885834500787410e-49), FLIT(0.0)},
    {FLIT(44.0), FLIT(2.4676780498616531735e-55), FLIT(0.0)},
    {FLIT(55.0), FLIT(4.8054910094320747983e-60), FLIT(0.0)},
    {FLIT(66.0), FLIT(6.3076208593219290698e-64), FLIT(0.0)},
    {FLIT(77.0), FLIT(3.1490831086709536890e-67), FLIT(0.0)},
    {FLIT(88.0), FLIT(4.2223904306022199285e-70), FLIT(0.0)},
    {FLIT(99.0), FLIT(1.2110718493018566369e-72), FLIT(0.0)},
};

typedef double lognormal_distribution_type;
lognormal_distribution<lognormal_distribution_type> lognormal_distribution_dist(-20.0, 500.0);
int lognormal_distribution_smaller_ok = 0;
int lognormal_distribution_larger_ok  = 1;

One_arg lognormal_distribution_vals[] = {
    {FLIT(0.0), FLIT(0.51595343685283079349), FLIT(0.0)},
    {FLIT(11.0), FLIT(6.9355142249137168195e-05), FLIT(0.0)},
    {FLIT(22.0), FLIT(3.5429500293984065392e-05), FLIT(0.0)},
    {FLIT(33.0), FLIT(2.3792899005689961879e-05), FLIT(0.0)},
    {FLIT(44.0), FLIT(1.7910450638045969073e-05), FLIT(0.0)},
    {FLIT(55.0), FLIT(1.4360112403848483223e-05), FLIT(0.0)},
    {FLIT(66.0), FLIT(1.1984448090128025368e-05), FLIT(0.0)},
    {FLIT(77.0), FLIT(1.0283225697318154478e-05), FLIT(0.0)},
    {FLIT(88.0), FLIT(9.0049387871531233869e-06), FLIT(0.0)},
    {FLIT(99.0), FLIT(8.0093079994869283491e-06), FLIT(0.0)},
};

typedef unsigned int negative_binomial_distribution_type;
negative_binomial_distribution<negative_binomial_distribution_type> negative_binomial_distribution_dist(500, 0.86);
int negative_binomial_distribution_smaller_ok = 0;
int negative_binomial_distribution_larger_ok  = 1;

One_arg negative_binomial_distribution_vals[] = {
    {FLIT(0.0), FLIT(1.7751114354849962758e-33), FLIT(0.0)},
    {FLIT(11.0), FLIT(9.8082107740810993668e-21), FLIT(0.0)},
    {FLIT(22.0), FLIT(9.7374291700601843825e-14), FLIT(0.0)},
    {FLIT(33.0), FLIT(4.4439009331195199142e-09), FLIT(0.0)},
    {FLIT(44.0), FLIT(6.4236461000209705694e-06), FLIT(0.0)},
    {FLIT(55.0), FLIT(0.00074357566972747151432), FLIT(0.0)},
    {FLIT(66.0), FLIT(0.011927027268930044667), FLIT(0.0)},
    {FLIT(77.0), FLIT(0.03809102681328114388), FLIT(0.0)},
    {FLIT(88.0), FLIT(0.031327529759048136638), FLIT(0.0)},
    {FLIT(99.0), FLIT(0.0080383675820337816691), FLIT(0.0)},
};

typedef double student_t_distribution_type;
student_t_distribution<student_t_distribution_type> student_t_distribution_dist(50);
int student_t_distribution_smaller_ok = 1;
int student_t_distribution_larger_ok  = 1;

One_arg student_t_distribution_vals[] = {
    {FLIT(0.0), FLIT(0.33893717744987772456), FLIT(0.0)},
    {FLIT(11.0), FLIT(2.8302556845392191973e-15), FLIT(0.0)},
    {FLIT(22.0), FLIT(9.8646847974168315934e-28), FLIT(0.0)},
    {FLIT(33.0), FLIT(5.0235340239736209210e-36), FLIT(0.0)},
    {FLIT(44.0), FLIT(4.0067552393902051078e-42), FLIT(0.0)},
    {FLIT(55.0), FLIT(6.3124603112886710814e-47), FLIT(0.0)},
    {FLIT(66.0), FLIT(6.9923295528041351860e-51), FLIT(0.0)},
    {FLIT(77.0), FLIT(3.0486632921900762651e-54), FLIT(0.0)},
    {FLIT(88.0), FLIT(3.6637663518242480349e-57), FLIT(0.0)},
    {FLIT(99.0), FLIT(9.6049691218903256753e-60), FLIT(0.0)},
};

typedef double weibull_distribution_type;
weibull_distribution<weibull_distribution_type> weibull_distribution_dist(12.0, 55.0);
int weibull_distribution_smaller_ok = 0;
int weibull_distribution_larger_ok  = 1;

One_arg weibull_distribution_vals[] = {
    {FLIT(0.0), FLIT(1.3051118293416215321e-21), FLIT(0.0)},
    {FLIT(11.0), FLIT(7.5405081073014867302e-09), FLIT(0.0)},
    {FLIT(22.0), FLIT(1.1823551312791094216e-05), FLIT(0.0)},
    {FLIT(33.0), FLIT(0.00093528583410685574294), FLIT(0.0)},
    {FLIT(44.0), FLIT(0.019648901274758198697), FLIT(0.0)},
    {FLIT(55.0), FLIT(0.078893724763520273192), FLIT(0.0)},
    {FLIT(66.0), FLIT(0.00011119556052245381853), FLIT(0.0)},
    {FLIT(77.0), FLIT(2.3884382333075901134e-25), FLIT(0.0)},
    {FLIT(88.0), FLIT(1.5132191001912599564e-124), FLIT(0.0)},
    {FLIT(99.0), FLIT(0.0), FLIT(0.0)},
};

int _Random_test_fun_1(double x) {
    return (int) x - 50;
}

typedef double piecewise_constant_distribution_type;
piecewise_constant_distribution<piecewise_constant_distribution_type> piecewise_constant_distribution_dist(
    8, 50.0, 100.0, _Random_test_fun_1);
int piecewise_constant_distribution_smaller_ok = 0;
int piecewise_constant_distribution_larger_ok  = 0;

One_arg piecewise_constant_distribution_vals[] = {
    {FLIT(0.0), FLIT(0.0), FLIT(0.0)},
    {FLIT(11.0), FLIT(0.0), FLIT(0.0)},
    {FLIT(22.0), FLIT(0.0), FLIT(0.0)},
    {FLIT(33.0), FLIT(0.0), FLIT(0.0)},
    {FLIT(44.0), FLIT(0.0), FLIT(0.0)},
    {FLIT(55.0), FLIT(0.0024489795918367346), FLIT(0.0)},
    {FLIT(66.0), FLIT(0.012244897959183673), FLIT(0.0)},
    {FLIT(77.0), FLIT(0.022857142857142857), FLIT(0.0)},
    {FLIT(88.0), FLIT(0.0326530612244898), FLIT(0.0)},
    {FLIT(99.0), FLIT(0.037551020408163265), FLIT(0.0)},
};

typedef double piecewise_linear_distribution_type;
piecewise_linear_distribution<piecewise_linear_distribution_type> piecewise_linear_distribution_dist(
    8, 50.0, 100.0, _Random_test_fun_1);
int piecewise_linear_distribution_smaller_ok = 0;
int piecewise_linear_distribution_larger_ok  = 0;

One_arg piecewise_linear_distribution_vals[] = {
    {FLIT(0.0), FLIT(0.0), FLIT(0.0)},
    {FLIT(11.0), FLIT(0.0), FLIT(0.0)},
    {FLIT(22.0), FLIT(0.0), FLIT(0.0)},
    {FLIT(33.0), FLIT(0.0), FLIT(0.0)},
    {FLIT(44.0), FLIT(0.0), FLIT(0.0)},
    {FLIT(55.0), FLIT(0.004467005076142132), FLIT(0.0)},
    {FLIT(66.0), FLIT(0.013401015228426396), FLIT(0.0)},
    {FLIT(77.0), FLIT(0.02233502538071066), FLIT(0.0)},
    {FLIT(88.0), FLIT(0.03126903553299493), FLIT(0.0)},
    {FLIT(99.0), FLIT(0.04020304568527919), FLIT(0.0)},
};

int _Random_test_fun_2(double x) {
    return (int) x;
}

typedef unsigned int discrete_distribution_type;
discrete_distribution<discrete_distribution_type> discrete_distribution_dist(100, 70, 90, _Random_test_fun_2);
int discrete_distribution_smaller_ok = 0;
int discrete_distribution_larger_ok  = 0;

One_arg discrete_distribution_vals[] = {
    {FLIT(0.0), FLIT(0.0088050314465408802827), FLIT(0.0)},
    {FLIT(11.0), FLIT(0.009056603773584906077), FLIT(0.0)},
    {FLIT(22.0), FLIT(0.0093081761006289301386), FLIT(0.0)},
    {FLIT(33.0), FLIT(0.0095597484276729559337), FLIT(0.0)},
    {FLIT(44.0), FLIT(0.0098113207547169817292), FLIT(0.0)},
    {FLIT(55.0), FLIT(0.010188679245283018688), FLIT(72510786453261.0)},
    {FLIT(66.0), FLIT(0.010440251572327044483), FLIT(0.0)},
    {FLIT(77.0), FLIT(0.010691823899371068543), FLIT(0.0)},
    {FLIT(88.0), FLIT(0.010943396226415094337), FLIT(0.0)},
    {FLIT(99.0), FLIT(0.011194968553459120135), FLIT(0.0)},
};
