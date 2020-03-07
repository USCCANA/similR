#include <Rcpp.h>
using namespace Rcpp;

#define a 0
#define b 1
#define c 2
#define d 3

template<typename T>
void vecprint(const std::vector< T > & x) {
  std::cout << "Here is a vector of length " << x.size() << "\n";
  for (int i = 0; i < x.size(); ++i)
    std::cout << x[i] << "\n";
  
  return;
  
}

typedef std::vector< int > vecint;

template<typename Ti, typename Tm> inline
void contingency_matrix(
    std::vector<Ti> & table,
    const Tm & M1,
    const Tm & M2,
    bool include_self,
    const vecint & exclude
  ) {
  
  // Catching error
  if ((M1.ncol() != M2.ncol()) | (M1.nrow() != M2.nrow())) 
    stop("`a` and `b` must have the same dimensions.");
  
  if (table.size() != 4)
    stop("`table` must be of size 4.");
  
  std::fill(table.begin(), table.end(), 0);
  
  int n = M1.nrow();
  int m = M1.ncol();
  
  // Set minus
  std::vector< int > rowidx(0); rowidx.reserve(n - exclude.size());
  std::vector< int > colidx(0); colidx.reserve(m - exclude.size());
  
  for (int i = 0; i < n; ++i) {
    if (std::find(exclude.begin(), exclude.end(), i) == exclude.end())
      rowidx.push_back(i);
  }
  
  for (int i = 0; i < m; ++i) {
    if (std::find(exclude.begin(), exclude.end(), i) == exclude.end())
      colidx.push_back(i);
  }

  for (auto i = rowidx.begin(); i != rowidx.end(); ++i) {
    
    for (auto j = colidx.begin(); j != colidx.end(); ++j) {
      
      if (!include_self && *i == *j)
        continue;
      
      else {
        if ((M1(*i, *j) == M2(*i, *j)) & (M1(*i, *j) == 1)) table[a]++;
        else if ((M1(*i, *j) != M2(*i, *j)) & (M1(*i, *j) == 1)) table[b]++;
        else if ((M1(*i, *j) != M2(*i, *j)) & (M1(*i, *j) == 0)) table[c]++;
        else if ((M1(*i, *j) == M2(*i, *j)) & (M1(*i, *j) == 0)) table[d]++;
      }
      
    }
  }
}


template<typename Ti, typename Tm> inline
std::vector<Ti> contingency_matrix(
    const Tm & M1, const Tm & M2, bool include_self, const vecint & exclude
  ) {
  
  std::vector<Ti> table(4);
  contingency_matrix< Ti, Tm >(table, M1, M2, include_self, exclude);
  
  return table;
  
}

//' Contingency Table
//' @param M1,M2 Two integer matrices of the same size.
//' @export
//' 
// [[Rcpp::export(rng = false)]]
IntegerMatrix contingency_matrix(
    const IntegerMatrix & M1,
    const IntegerMatrix & M2,
    bool include_self,
    const std::vector< int > & exclude
) {
  
  if (M1.nrow() != M2.nrow())
    stop("Number of rows don't match.");
  
  if (M1.ncol() != M2.ncol())
    stop("Number of columns don't match.");
  
  std::vector<double> table = contingency_matrix<double, IntegerMatrix>(
    M1, M2, include_self, exclude
    );
  
  IntegerMatrix ans(2,2);
  
  ans(0, 0) = table[a];
  ans(0, 1) = table[b];
  ans(1, 0) = table[c];
  ans(1, 1) = table[d];
  
  return ans;
  
}

//' @name similarity
//' @rdname similarity
//' @section Similarity:
//' - Jaccard (1): `"sjaccard"` or `"jaccard"`
//' @aliases Jaccard
double sjaccard(
    const IntegerMatrix & M1,
    const IntegerMatrix & M2,
    const vecint & exclude,
    bool include_self = false,
    bool normalized = false
) {
  
  std::vector<double> table = contingency_matrix<double, IntegerMatrix>(M1, M2, include_self, exclude);
  
  return table[a]/(table[a] + table[b] + table[c]);
  
}


//' @name similarity
//' @rdname similarity
//' @section Similarity:
//' - Faith (10): `"sfaith"` or `"faith"`
//' @aliases Faith
double sfaith(
    const IntegerMatrix & M1,
    const IntegerMatrix & M2,
    const vecint & exclude,
    bool include_self = false,
    bool normalized = false
) {
  
  std::vector<double> table = contingency_matrix<double, IntegerMatrix>(M1, M2, include_self, exclude);
  
  return (table[a] + table[d]*0.5)/(table[a] + table[b] + table[c] + table[d]);
  
}

//' @name similarity
//' @rdname similarity
//' @section Similarity:
//' - Gower and Legendre (11): `"sgl"` or `"gl"`
//' @aliases Gower-&-Legendre
double sgl(
    const IntegerMatrix & M1,
    const IntegerMatrix & M2,
    const vecint & exclude,
    bool include_self = false,
    bool normalized = false
) {
  
  std::vector<double> table = contingency_matrix<double, IntegerMatrix>(M1, M2, include_self, exclude);
  
  return (table[a] + table[d])/(table[a] + 0.5*(table[b] + table[c]) + table[d]);
  
}

//' @name similarity
//' @rdname similarity
//' @section Distance:
//' - Sized Difference (24): `"dsd"` or `"sd"`
//' @aliases Sized-Difference
double dsd(
    const IntegerMatrix & M1,
    const IntegerMatrix & M2,
    const vecint & exclude,
    bool include_self = false,
    bool normalized = false
) {
  
  std::vector<double> table = contingency_matrix<double, IntegerMatrix>(M1, M2, include_self, exclude);
  
  return pow(table[b] + table[c], 2.0)/
    pow(table[a] + table[b] + table[c] + table[d], 2.0);
  
}

//' @name similarity
//' @rdname similarity
//' @section Distance:
//' - Shaped Difference (25): `"dsphd"` or `"sphd"`
//' @aliases Shape-Difference
double dsphd(
    const IntegerMatrix & M1,
    const IntegerMatrix & M2,
    const vecint & exclude,
    bool include_self = false,
    bool normalized = false
) {
  
  std::vector<double> table = contingency_matrix<double, IntegerMatrix>(M1, M2, include_self, exclude);
  
  return (((double) M1.nrow())*(table[b] + table[c]) - pow(table[b] - table[c], 2.0))/
    pow(table[a] + table[b] + table[c] + table[d], 2.0);
  
}

//' @name similarity
//' @rdname similarity
//' @section Similarity:
//' - Tarwid (54): `"starwid"` or `"tarwid"`.
//' @aliases Tarwid
double starwid(
    const IntegerMatrix & M1,
    const IntegerMatrix & M2,
    const vecint & exclude,
    bool include_self = false,
    bool normalized = false
  ) {
  
  std::vector<double> table = contingency_matrix<double, IntegerMatrix>(M1, M2, include_self, exclude);
  int n = M1.nrow();
  return (n*table[a] - (table[a] + table[b])*(table[a] + table[c]))/
          (n*table[a] + (table[a] + table[b])*(table[a] + table[c]));
  
}

//' @name similarity
//' @rdname similarity
//' @section Similarity:
//' -  Pearson & Heron 1 (54): `"sph1"` or `"ph1"` or `"s14"`. This is also known as S14 in
//'    Gower and Legendre (1986).
//'    
//'    In the case of the `S14` function, following Krackhardt's 1989:
//'    
//'    \deqn{%
//'    \sqrt{\left(\frac{a}{(a + c)} - \frac{b}{(b + d)}\right)\times\left(\frac{a}{(a + b)} - \frac{c}{(c + d)}\right)}
//'    }{%
//'    S14 = [(a/(a + c) - b/(b + d))*(a/(a + b) - c/(c + d))]^(1/2)
//'    }
//'   
//'    Which is an statistic lying between 0 and 1.
//'  
//' @aliases Person-&-Heron
//' @aliases S14
double sph1(
    const IntegerMatrix & M1,
    const IntegerMatrix & M2,
    const vecint & exclude,
    bool include_self = false,
    bool normalized = false
  ) {
  
  std::vector<double> table = contingency_matrix<double, IntegerMatrix>(M1, M2, include_self, exclude);
  
  // std::cout << table[a] << table[b] << table[c] << "\n";
  
  return (table[a]*table[d] - table[b]*table[c])/
    sqrt((table[a] + table[b])*(table[a] + table[c])*(table[b] + table[d])*(table[c] + table[d]));
        
}

//' @name similarity
//' @rdname similarity
//' @section Distance:
//' - Hamming (15): `"dhamming"` or `"hamming"`
//' @aliases Hamming
double dhamming(
    const IntegerMatrix & R1,
    const IntegerMatrix & R2,
    const vecint & exclude,
    bool include_self = false,
    bool normalized = false
  ) {
  
  double ans = 0;
  int n = R1.nrow(), m = R1.ncol();
  for (int i = 0; i<n; ++i) 
    for (int j = 0; j<m; ++j) {
      if (i == j)
        continue;
      
      if (R1(i,j) != R2(i,j))
        ans++;
      
    }
  
  if (normalized) {
    double dn = (double) n;
      ans /= (dn*(dn - 1.0)) ;
  }
    
  return ans;
  
}

//' @name similarity
//' @rdname similarity
//' @section Similarity:
//' -  Mean Manhattan (20): `"dmh"` or `"mh"`
//'    \deqn{%
//'      D_{Mean-manhattan} = \frac{b + c}{a + b + c + d}
//'    }{%
//'     dmh = (b + c)/(a + b + c + d)
//'    }
//' @aliases Mean-Manhattan
double dmh(
    const IntegerMatrix & M1,
    const IntegerMatrix & M2,
    const vecint & exclude,
    bool include_self = false,
    bool normalized = false
) {
  
  std::vector<double> table = contingency_matrix<double, IntegerMatrix>(M1, M2, include_self, exclude);
  
  return (table[b]+table[c])/
    (table[a] + table[b] + table[c] + table[d]);
    
}

//' @name similarity
//' @rdname similarity
//' @section Similarity:
//' - Dennis (44): `"sdennis"` or `"dennis"`
//' @aliases Dennis
double sdennis(
    const IntegerMatrix & M1,
    const IntegerMatrix & M2,
    const vecint & exclude,
    bool include_self = false,
    bool normalized = false
) {
  
  std::vector<double> table = contingency_matrix<double, IntegerMatrix>(M1, M2, include_self, exclude);
  
  double n = (double) M1.nrow();
  
  return (table[a]*table[d] - table[b]*table[c])/
    sqrt(n*(table[a] + table[b])*(table[a] + table[c]));
  
}

//' @name similarity
//' @rdname similarity
//' @section Similarity:
//' - Yuleq (61): `"syuleq"`
//' @aliases Yuleq
double syuleq(
    const IntegerMatrix & M1,
    const IntegerMatrix & M2,
    const vecint & exclude,
    bool include_self = false,
    bool normalized = false
) {
  
  std::vector<double> table = contingency_matrix<double, IntegerMatrix>(M1, M2, include_self, exclude);
  
  return (table[a]*table[d] - table[b]*table[c])/(table[a]*table[d] + table[b]*table[c]);
  
}

//' @name similarity
//' @rdname similarity
//' @section Similarity:
//' - Yuleq (63): `"syuleqw"`
//' @aliases Yuleq
double syuleqw(
    const IntegerMatrix & M1,
    const IntegerMatrix & M2,
    const vecint & exclude,
    bool include_self = false,
    bool normalized = false
) {
  
  std::vector<double> table = contingency_matrix<double, IntegerMatrix>(M1, M2, include_self, exclude);
  
  return (sqrt(table[a]*table[d]) - sqrt(table[b]*table[c]))/
    (sqrt(table[a]*table[d]) + sqrt(table[b]*table[c]));
  
}

//' @name similarity
//' @rdname similarity
//' @section Distance:
//' - Yuleq (62): `"dyuleq"`
//' @aliases Yuleq
double dyuleq(
  const IntegerMatrix & M1,
  const IntegerMatrix & M2,
  const vecint & exclude,
  bool include_self = false,
  bool normalized = false
) {
  
  std::vector<double> table = contingency_matrix<double, IntegerMatrix>(M1, M2, include_self, exclude);
  
  return 2.0*table[b]*table[c]/(table[a]*table[d] + table[b]*table[c]);
  
}

//' @name similarity
//' @rdname similarity
//' @section Similarity:
//' -  Michael (68):  `"smichael"` or `"michael"`
//'    
//'    \deqn{%
//'      S_{michael} = \frac{4(ad-bc)}{(a+d)^2 + (b+c)^2}
//'    }{%
//'      smichael = 4*(a*d - b*c)/[(a + d)^2 + (b + c)^2]
//'    }
//' @aliases Michael
double smichael(
  const IntegerMatrix & M1,
  const IntegerMatrix & M2,
  const vecint & exclude,
  bool include_self = false,
  bool normalized = false
) {
  
  std::vector<double> table = contingency_matrix<double, IntegerMatrix>(M1, M2, include_self, exclude);
  
  return 4.0*(table[a]*table[d] - table[b]*table[c])/
    (pow(table[a] + table[d], 2.0) + pow(table[b] + table[c], 2.0));
}

//' @name similarity
//' @rdname similarity
//' @section Similarity:
//' -  Dispersion (66):  `"sdisp"` or `"disp"`
//'    
//'    \deqn{%
//'      S_{Dispersion} = \frac{ad - bc}{(a + b + c + d)^2}
//'    }{%
//'      sdisp = [a*d - b*c]/(a + b + c + d)^2
//'    }
//' @aliases Dispersion
double sdisp(
    const IntegerMatrix & M1,
    const IntegerMatrix & M2,
    const vecint & exclude,
    bool include_self = false,
    bool normalized = false
) {
  
  std::vector<double> table = contingency_matrix<double, IntegerMatrix>(M1, M2, include_self, exclude);
  
  return (table[a]*table[d] - table[b] * table[c])/
    pow(table[a] + table[b] + table[c] + table[d], 2.0);
  
}


//' @name similarity
//' @rdname similarity
//' @section Similarity:
//' -  Hamann (67):  `"shamann"` or `"hamann"`
//'    
//'    \deqn{%
//'      S_{Hamann} = \frac{(a + d) - (b + c)}{a + b + c + d}
//'    }{%
//'      shamann = [(a + d) - (b + c)](a + b + c + d)
//'    }
//' @aliases Hamann
double shamann(
    const IntegerMatrix & M1,
    const IntegerMatrix & M2,
    const vecint & exclude,
    bool include_self = false,
    bool normalized = false
) {
  
  std::vector<double> table = contingency_matrix<double, IntegerMatrix>(M1, M2, include_self, exclude);
  
  return ((table[a] + table[d]) - (table[b] - table[c]))/
    (table[a] + table[b] + table[c] + table[d]);
  
}

// Auxiliar functions for Goodman & Kruskal, and Anderberg
inline double sigma(const std::vector<double> & table) {
  
  return std::max(table[a], table[b]) + std::max(table[c], table[d]) + 
    std::max(table[a], table[c]) + std::max(table[b], table[d]);
  
}

inline double sigma_prime(const std::vector<double> & table) {
  
  return std::max(table[a] + table[c], table[b] + table[d]) +
    std::max(table[a] + table[b], table[c] + table[d]);
  
}

//' @name similarity
//' @rdname similarity
//' @section Similarity:
//' -  Goodman & Kruskal (69): `"sgk"` or `"gk"`
//'    
//'    \deqn{%
//'      S_{Goodman \& Kruskal} = \frac{\sigma - \sigma'}{2n - \sigma'} 
//'    }{%
//'      sgk = (s + s')/(2*n - s') 
//'    }
//'    
//'    where
//'    \eqn{\sigma = \max(a,b) + \max(c,d) + \max(a,c) + \max(b,d)}{
//'    s = max(a,b) + max(c,d) + max(a,c) + max(b,d)
//'    }, and 
//'    \eqn{\sigma' = \max(a + c, b + d) + \max(a + b, c + d)}{
//'    s' = max(a + c, b + d) + max(a + b, c + d)
//'    }
//'   
//' @aliases Goodman-&-Kruskal
double sgk(
    const IntegerMatrix & M1,
    const IntegerMatrix & M2,
    const vecint & exclude,
    bool include_self = false,
    bool normalized = false
) {
  
  std::vector<double> table = contingency_matrix<double, IntegerMatrix>(M1, M2, include_self, exclude);
  double s = sigma(table);
  double s_prime = sigma_prime(table);
  
  return (s - s_prime)/(2.0*(double) M1.nrow() - s_prime);
  
}

//' @name similarity
//' @rdname similarity
//' @section Similarity:
//' -  Anderberg (70): `"sanderberg"` or `"anderberg"`
//'    
//'    \deqn{%
//'      S_{Anderberg} = \frac{\sigma - \sigma'}{2n} 
//'    }{%
//'      sanderberg = (s + s')/(2*n) 
//'    }
//'    
//'    where \eqn{\sigma}{s} and \eqn{\sigma}{s'} are defined as in (69).
//'   
//' @aliases Anderberg
double sanderberg(
    const IntegerMatrix & M1,
    const IntegerMatrix & M2,
    const vecint & exclude,
    bool include_self = false,
    bool normalized = false
) {
  
  std::vector<double> table = contingency_matrix<double, IntegerMatrix>(M1, M2, include_self, exclude);
  double s = sigma(table);
  double s_prime = sigma_prime(table);
  
  return (s - s_prime)/(2.0*(double) M1.nrow());
  
}





//' @name similarity
//' @rdname similarity
//' @section Similarity:
//' -  Peirce (73): `"speirce"` or `"peirce"`
//'    
//'    \deqn{%
//'      S_{Peirce} = \frac{ab + bc}{ab + 2bc + cd}
//'    }{%
//'      speirce = (a*b + b*c)/(a*b + 2*b*c + c*d)
//'    }
//'   
//' @aliases Peirce
double speirce(
  const IntegerMatrix & M1,
  const IntegerMatrix & M2,
  const vecint & exclude,
  bool include_self = false,
  bool normalized = false
) {
  
  std::vector<double> table = contingency_matrix<double, IntegerMatrix>(M1, M2, include_self, exclude);
  
  return (table[a]*table[b] + table[b]*table[c])/
    (table[a]*table[b] + 2*table[b]*table[c] + table[c]*table[d]);
  
}

//' @name similarity
//' @rdname similarity
//' @aliases Fscore
//' @section Similarity: 
//' Ask Kyosuke Tanaka
// F score
double fscore(
    const IntegerMatrix & M1,
    const IntegerMatrix & M2,
    const vecint & exclude,
    bool include_self = false,
    bool normalized = false
) {
  
  std::vector<double> table = contingency_matrix<double, IntegerMatrix>(M1, M2, include_self, exclude);
  
  double precision = table[a]/(table[a] + table[c]);
  double recall    = table[a]/(table[b] + table[a]);
  
  return 2.0 * (precision * recall / (precision + recall));
  
}

// -----------------------------------------------------------------------------

typedef double (*funcPtr)(
    const IntegerMatrix & M1,
    const IntegerMatrix & M2,
    const vecint & exclude,
    bool include_self,
    bool normalized
  );

// [[Rcpp::export(rng = false)]]
IntegerMatrix reduce_dim(IntegerMatrix & x, int k) {
  
  // No reduction, return the same
  if (k == -1)
    return x;
  
  // Preparing reduced version of the matrix
  IntegerMatrix ans(x.nrow() - 1, x.ncol() - 1);
  ans.fill(0);
  
  int i0=0, j0;
  for (int i = 0; i < x.nrow(); ++i) {
    
    if (i == k)
      continue;

    j0 = 0;    
    for (int j = 0; j < x.ncol(); ++j) {
      
      // Self
      if (j == k) 
        continue;
      
      if (x(i,j) == 1)
        ans(i0, j0) = 1;

      // Out of range (can't do more)      
      if (++j0 == ans.ncol())
        break;
     
    }
    
    // Out of range (can't do more)
    if (++i0 == ans.nrow())
      break;
  }
  
  return ans;
  
}

void getmetric(std::string s, funcPtr & fun) {
  
  if      ((s == "sph1") | (s == "ph1") | (s == "s14")) fun = &sph1;
  else if ((s == "dhamming") | (s == "hamming"))      fun = &dhamming;
  else if ((s == "dennis") | (s == "sdennis"))        fun = &sdennis;
  else if ((s == "starwid") | (s == "tarwid"))        fun = &starwid;
  else if (s == "syuleq")                         fun = &syuleq;
  else if (s == "syuleqw")                        fun = &syuleqw;
  else if (s == "dyuleq")                         fun = &dyuleq;
  else if ((s == "smichael") | (s == "michael"))      fun = &smichael;
  else if ((s == "speirce") | (s == "peirce"))        fun = &speirce;
  else if ((s == "sjaccard") | (s == "jaccard"))      fun = &sjaccard;
  else if ((s == "sgk") | (s == "gyk"))               fun = &sgk;
  else if ((s == "sanderberg") | (s == "anderberg"))  fun = &sanderberg;
  else if ((s == "shamann") | (s == "hamann"))        fun = &shamann;
  else if ((s == "dmh") | (s == "mh"))                fun = &dmh;
  else if ((s == "sfaith") | (s == "faith"))          fun = &sfaith;
  else if ((s == "sgl") | (s == "gl"))                fun = &sgl;
  else if ((s == "dsd") | (s == "sd"))                fun = &dsd;
  else if ((s == "dsphd") | (s == "sphd"))            fun = &dsphd;
  else if ((s == "sdisp") | (s == "disp"))            fun = &sdisp;
  else if ((s == "fscore") | (s == "Fscore"))         fun = &fscore;
  else Rcpp::stop("The statistic '%s' is not defined.", s);
  
  return ;
}

// Applies whatever similarity/distance metric should be applying to all the
// requested combinations
// [[Rcpp::export(name=".similarity", rng = false)]]
NumericMatrix similarity(
    const ListOf<IntegerMatrix> & M,
    const std::vector< std::string > & statistic,
    bool normalized   = false,
    bool firstonly    = false,
    bool include_self = false,
    bool exclude_j    = false
) {
  
  int N  = M.size();
  int NN = firstonly? N-1 : N*(N-1)/2;
  int nfuns = statistic.size();
  NumericVector ans(NN * nfuns);
  NumericVector I(NN * nfuns),J(NN * nfuns),S(NN * nfuns);
  
  int pos = 0;
  
  std::vector< int > exclude(0u);
  
  int firstloop = firstonly ? 1 : N;  
  
  if (exclude_j)
    exclude.push_back(0);
  
  for (int i = 0; i < firstloop; ++i) {
    for (int j = i; j < N; ++j) {
      
      if (exclude_j)
        exclude[0] = j;
      
      // Getting the pointers
      IntegerMatrix Mi = M[i];
      IntegerMatrix Mj = M[j];
      
      if (i == j)
        continue;
      int s = 0;
      for (auto fname = statistic.begin(); fname != statistic.end(); ++fname) {
        
        // Getting the function
        funcPtr fun;
        getmetric(*fname, fun);
      
        I[pos] = i + 1;
        J[pos] = j + 1;
        S[pos] = s++;
        ans[pos++] = fun(M[i], M[j], exclude, include_self, normalized);
      }
      
    }
  }
  
  
  return cbind(S,I,J,ans);
  
}

// // [[Rcpp::export(name=".similarity", rng = false)]]
// NumericMatrix similarity(
//     const ListOf<IntegerMatrix> & M,
//     const std::string & statistic,
//     bool normalized   = false,
//     bool firstonly    = false,
//     bool include_self = false,
//     bool exclude_j    = false
//   ) {
//   
//   funcPtr fun;
//   getmetric(statistic, fun);
//   
//   return allsimilarities(M, normalized, fun, firstonly, include_self, exclude_j);
//   
// }

// No longer needed
#undef a
#undef b
#undef c
#undef d
