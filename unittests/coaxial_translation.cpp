#include "catch.hpp"

#include "Bessel.h"
#include "CoAxialTranslationCoefficients.h"
#include "constants.h"
#include <Coefficients.h>
#include <boost/math/special_functions/bessel.hpp>
#include <boost/math/special_functions/spherical_harmonic.hpp>
#include <cmath>
#include <iostream>
#include <random>

extern std::unique_ptr<std::mt19937_64> mersenne;

using namespace optimet;

void check_coaxial_n_recurrence(CachedCoAxialRecurrence tca, t_int n, t_int m, t_int l) {
  // This aims to check that we correctly implement formula 4.79
  using coefficient::a;
  auto left0 = a(n - 1, m) * static_cast<t_complex>(tca(n - 1, m, l)) -
               a(n, m) * static_cast<t_complex>(tca(n + 1, m, l));
  auto right0 = a(l, m) * static_cast<t_complex>(tca(n, m, l + 1)) -
                a(l - 1, m) * static_cast<t_complex>(tca(n, m, l - 1));
  INFO("Testing n recurrence for n " << n << " m " << m << " l " << l);
  CHECK(left0.real() == Approx(right0.real()));
  CHECK(left0.imag() == Approx(right0.imag()));
}

void check_coaxial_m_recurrence(CachedCoAxialRecurrence tca, t_int n, t_int m, t_int l) {
  // This aims to check that we correctly implement formula 4.80
  using coefficient::b;
  auto left0 = b(n, m) * static_cast<t_complex>(tca(n - 1, m + 1, l)) -
               b(n + 1, -m - 1) * static_cast<t_complex>(tca(n + 1, m + 1, l));
  auto right0 = b(l + 1, m) * static_cast<t_complex>(tca(n, m, l + 1)) -
                b(l, -m - 1) * static_cast<t_complex>(tca(n, m, l - 1));
  INFO("Testing m recurrence for n " << n << " m " << m << " l " << l);
  CHECK(left0.real() == Approx(right0.real()));
  CHECK(left0.imag() == Approx(right0.imag()));
}

void check_coaxial_mn_recurrence(CachedCoAxialRecurrence tca, t_int n, t_int m, t_int l) {
  // This aims to check that we correctly implement formula 4.84
  using coefficient::b;
  auto left0 = b(n + 1, -m - 1) * static_cast<t_complex>(tca(n + 1, m + 1, l));
  auto right0 = b(l, -m - 1) * static_cast<t_complex>(tca(n, m, l - 1)) -
                b(l + 1, m) * static_cast<t_complex>(tca(n, m, l + 1));
  INFO("Testing m=n recurrence for n " << n << " m " << m << " l " << l);
  CHECK(left0.real() == Approx(right0.real()));
  CHECK(left0.imag() == Approx(right0.imag()));
}

void check_coaxial_m_symmetry(CachedCoAxialRecurrence tca, t_int n, t_int m, t_int l) {
  // check that value is independent of m
  auto left0 = tca(n, m, l);
  auto right0 = tca(n, -m, l);
  INFO("Testing m symmetry for " << n << " m " << m << " l " << l);
  CHECK(left0.real() == Approx(right0.real()));
  CHECK(left0.imag() == Approx(right0.imag()));
}

void check_coaxial_ln_symmetry(CachedCoAxialRecurrence tca, t_int n, t_int m, t_int l) {
  // check that value has the right sign change for m<->l
  auto left0 = static_cast<t_complex>(tca(n, m, l));
  t_complex sign = ((n + l) % 2 == 0 ? 1 : -1);
  auto right0 = static_cast<t_complex>(tca(l, m, n)) * sign;
  INFO("Testing l to n symmetry n " << n << " m " << m << " l " << l);
  CHECK(left0.real() == Approx(right0.real()));
  CHECK(left0.imag() == Approx(right0.imag()));
}

TEST_CASE("CoAxial") {
  SECTION("Initial values") {
    using coefficient::a;
    using coefficient::b;
    t_real const R(1e0);
    t_complex const waveK(1e0, 1.5e0);
    CachedCoAxialRecurrence tca(R, waveK, true);
    // Numbers are generated from the same formula in Scipy
    CHECK(static_cast<t_complex>(tca(0, 0, 0)).real() == Approx(1.1400511799225792));
    CHECK(static_cast<t_complex>(tca(0, 0, 0)).imag() == Approx(-0.55962217045848206));
    CHECK(static_cast<t_complex>(tca(0, 0, 4)).real() == Approx(-0.028191522402192234));
    CHECK(static_cast<t_complex>(tca(0, 0, 4)).imag() == Approx(-0.02162885905593049));
    CHECK(static_cast<t_complex>(tca(1, 0, 1)).real() == Approx(1.2274819687880665));
    CHECK(static_cast<t_complex>(tca(1, 0, 1)).imag() == Approx(-1.0271756758800463));
    CHECK(static_cast<t_complex>(tca(-1, 1, 3)).real() == Approx(0));
    CHECK(static_cast<t_complex>(tca(-1, 1, 3)).imag() == Approx(0));
    CHECK(static_cast<t_complex>(tca(1, 0, -1)).real() == Approx(0));
    CHECK(static_cast<t_complex>(tca(1, 0, -1)).imag() == Approx(0));
    CHECK(static_cast<t_complex>(tca(1, 1, 3)).real() == Approx(-0.085169586217943016));
    CHECK(static_cast<t_complex>(tca(1, 1, 3)).imag() == Approx(0.36331568009355053));

    t_complex expected;
    expected = (static_cast<t_complex>(tca(1, 0, 1)) * a(1, 0) +
                static_cast<t_complex>(tca(0, 0, 2)) * a(0, 0) -
                static_cast<t_complex>(tca(1, 0, 3)) * a(2, 0)) /
               (a(1, 0));
    CHECK(static_cast<t_complex>(tca(2, 0, 2)).real() == Approx(expected.real()));
    CHECK(static_cast<t_complex>(tca(2, 0, 2)).imag() == Approx(expected.imag()));

    expected = (static_cast<t_complex>(tca(0, 0, 2)) * a(2, 0) -
                static_cast<t_complex>(tca(0, 0, 4)) * a(3, 0)) /
               (a(0, 0));
    CHECK(static_cast<t_complex>(tca(1, 0, 3)).real() == Approx(expected.real()));
    CHECK(static_cast<t_complex>(tca(1, 0, 3)).imag() == Approx(expected.imag()));

    expected = (static_cast<t_complex>(tca(1, 0, 3)) * a(3, 0) +
                static_cast<t_complex>(tca(0, 0, 4)) * a(0, 0) -
                static_cast<t_complex>(tca(1, 0, 5)) * a(4, 0)) /
               (a(1, 0));
    CHECK(static_cast<t_complex>(tca(2, 0, 4)).real() == Approx(expected.real()));
    CHECK(static_cast<t_complex>(tca(2, 0, 4)).imag() == Approx(expected.imag()));

    expected = (-static_cast<t_complex>(tca(0, 0, 2)) * b(3, -1) +
                static_cast<t_complex>(tca(0, 0, 4)) * b(4, 0)) /
               (-b(1, -1));
    CHECK(static_cast<t_complex>(tca(1, 1, 3)).real() == Approx(expected.real()));
    CHECK(static_cast<t_complex>(tca(1, 1, 3)).imag() == Approx(expected.imag()));

    expected = (-static_cast<t_complex>(tca(2, 0, 2)) * b(3, -1) -
                static_cast<t_complex>(tca(1, 1, 3)) * b(2, 0) +
                static_cast<t_complex>(tca(2, 0, 4)) * b(4, 0)) /
               (-b(3, -1));
    CHECK(static_cast<t_complex>(tca(3, 1, 3)).real() == Approx(expected.real()));
    CHECK(static_cast<t_complex>(tca(3, 1, 3)).imag() == Approx(expected.imag()));
  }
  SECTION("R zero") {
    using coefficient::a;
    using coefficient::b;
    t_real const R(0.0);
    t_complex const waveK(1e0, 0.0);
    CachedCoAxialRecurrence tca(R, waveK, true);
    // Check that the simple 0,0 coeff is as expected. This revealed
    // an issue in the bessel function implementation for (0,0)
    CHECK(static_cast<t_complex>(tca(0, 0, 0)).real() == Approx(1.0));
    CHECK(static_cast<t_complex>(tca(0, 0, 0)).imag() == Approx(0.0));
    CHECK(std::get<0>(optimet::bessel<Bessel>(0, 0))[0].real() == Approx(1.0));
  }
  SECTION("Check n and m recurrence") {
    t_complex const waveK(1e0, 1.5e0);
    CachedCoAxialRecurrence tca(1.0, waveK, true);
    t_int max_recur = 10;
    for(t_int l = 0; l < max_recur; ++l) {
      for(t_int n = 0; n < max_recur; ++n) {
        check_coaxial_mn_recurrence(tca, n, n, l);
        for(t_int m = -n; m <= n; ++m) {
          check_coaxial_ln_symmetry(tca, n, m, l);
          check_coaxial_m_symmetry(tca, n, m, l);
          check_coaxial_n_recurrence(tca, n, m, l);
          check_coaxial_m_recurrence(tca, n, m, l);
        }
      }
    }
  }
}

void check_coaxial_translation_zero(t_int n, t_int m, t_complex waveK, bool regular) {
  assert(m <= n);
  bool coeffs_regular;
  std::uniform_real_distribution<> theta_dist(0, consPi / 2);
  std::uniform_real_distribution<> phi_dist(0, 2 * consPi);
  std::uniform_real_distribution<> r_p_dist(1, 10);
  t_real r_p = r_p_dist(*mersenne);
  t_real theta_p = theta_dist(*mersenne);
  t_real phi = phi_dist(*mersenne);

  t_real r_pq = 0.0;

  t_real theta_q = theta_p;
  t_real r_q = r_p;

  auto basis_func = optimet::bessel<Bessel>;
  auto re_basis_func = optimet::bessel<Bessel>;
  coeffs_regular = true;
  if(regular == false) {
    basis_func = optimet::bessel<Hankel1>;
    re_basis_func = optimet::bessel<Hankel1>;
  }

  CachedCoAxialRecurrence tca(r_pq, waveK, coeffs_regular);
  t_complex translated = 0;
  for(t_int l = std::abs(m); l < std::abs(m) + 105; l++) {
    translated += static_cast<t_complex>(tca(n, m, l)) *
                  std::get<0>(re_basis_func(r_q * waveK, l)).back() *
                  boost::math::spherical_harmonic(l, m, theta_q, phi);
  }
  auto expected = std::get<0>(basis_func(r_p * waveK, n)).back() *
                  boost::math::spherical_harmonic(n, m, theta_p, phi);
  INFO("Testing translation for n: " << n << " m: " << m << " sph "
                                     << boost::math::spherical_harmonic(n, m, theta_p, phi));
  CHECK(expected.real() == Approx(translated.real()));
  CHECK(expected.imag() == Approx(translated.imag()));
}

void check_coaxial_translation_onaxis(t_real expansion_pos, t_real reexpansion_pos,
                                      bool expansion_regular, bool reexpansion_regular, t_int n,
                                      t_int m, t_complex waveK) {
  assert(!(expansion_regular and !reexpansion_regular));
  assert(m <= n);
  bool coeff_regular = expansion_regular == reexpansion_regular;
  t_real const translation(expansion_pos - reexpansion_pos);
  CachedCoAxialRecurrence tca(translation, waveK, coeff_regular);
  auto const basis_func = expansion_regular ? optimet::bessel<Bessel> : optimet::bessel<Hankel1>;
  auto const re_basis_func =
      reexpansion_regular ? optimet::bessel<Bessel> : optimet::bessel<Hankel1>;
  t_complex translated = 0;
  for(t_int l = std::abs(m); l < std::abs(m) + 25; l++) {
    translated += static_cast<t_complex>(tca(n, m, l)) *
                  std::get<0>(re_basis_func(reexpansion_pos * waveK, l)).back() *
                  boost::math::spherical_harmonic(l, m, 0, 0);
  }
  auto expected = std::get<0>(basis_func(expansion_pos * waveK, n)).back() *
                  boost::math::spherical_harmonic(n, m, 0, 0);
  INFO("Testing translation for n: " << n << " m: " << m << " regular: " << expansion_regular
                                     << " to: " << reexpansion_regular);
  CHECK(expected.real() == Approx(translated.real()));
  CHECK(expected.imag() == Approx(translated.imag()));
}


void check_coaxial_translation_off_axis_reexpand_iregular(t_int n, t_int m, t_complex waveK) {
  assert(m <= n);
  //! Inner floating point with higher precision
  typedef long double Real;
  //! Inner complex floating point with higher precision
  typedef std::complex<Real> Complex;
  bool coeffs_regular;
  std::uniform_real_distribution<> theta_dist(0, consPi / 2);
  std::uniform_real_distribution<> phi_dist(0, 2 * consPi);
  std::uniform_real_distribution<> r_p_dist(1, 10);
  auto const r_p = r_p_dist(*mersenne);
  auto const theta_p = theta_dist(*mersenne);
  auto const phi = phi_dist(*mersenne);
  auto theta_p1 = theta_p;
  if(theta_p > consPi / 2)
    // internal angle in the triangle < pi/2
    theta_p1 = consPi - theta_p;

  t_real z_p = cos(theta_p1) * r_p;
  std::uniform_real_distribution<> r_pq_dist(-10, 10);
  t_real r_pq = 1;
  t_real r_q = 1;
  t_real rho_p;
  t_real rho_q;
  t_real z_q;
  t_real theta_q;
  t_int n1 = 0;
  r_pq = r_pq_dist(*mersenne);
  rho_p = sin(theta_p1) * r_p;
  rho_q = rho_p;
  z_q = 0;

  if((theta_p <= consPi / 2))
    z_q = std::abs(z_p - r_pq);
  else
    z_q = std::abs(z_p + r_pq);
  theta_q = atan(rho_q / z_q);
  r_q = z_q / cos(theta_q);
  n1 = n1 + 1;

  CHECK(sin(theta_p1) * r_p == Approx(sin(theta_q) * r_q));
  if((theta_p <= consPi / 2) and (r_pq > z_p))
    // swap to get the angle from positive z axis
    theta_q = consPi - theta_q;
  if((theta_p > consPi / 2) and (r_pq > -z_p))
    theta_q = consPi - theta_q;
  auto const basis_func = optimet::bessel<Hankel1>;
  auto re_basis_func = optimet::bessel<Bessel>;
  if(std::abs(r_q) <= std::abs(r_pq)) {
    coeffs_regular = false;
    re_basis_func = optimet::bessel<Bessel>;
  } else {
    coeffs_regular = true;
    re_basis_func = optimet::bessel<Hankel1>;
  }
  CachedCoAxialRecurrence tca(r_pq, waveK, coeffs_regular);
  Complex translated = 0;
  for(t_int l = std::abs(m); l < std::abs(m) + 105; l++) {
    translated += static_cast<t_complex>(tca(n, m, l)) *
                  std::get<0>(re_basis_func(r_q * waveK, l)).back() *
                  boost::math::spherical_harmonic(l, m, theta_q, phi);
  }
  auto expected = std::get<0>(basis_func(r_p * waveK, n)).back() *
                  boost::math::spherical_harmonic(n, m, theta_p, phi);
  INFO("Testing translation for regular ? 0 "
       << " reexpanded in regular? " << !coeffs_regular << " n: " << n << " m: " << m << " sph "
       << boost::math::spherical_harmonic(n, m, theta_p, phi) << " bessel "
       << std::get<0>(basis_func(r_p * waveK, n)).back() << r_p * waveK << " r_p " << r_p << " r_q "
       << r_q << " r_pq " << r_pq);
  CHECK(expected.real() == Approx(translated.real()));
  CHECK(expected.imag() == Approx(translated.imag()));
}

TEST_CASE("Coaxial translation") {
  std::uniform_real_distribution<> small_dist(0, 1);
  std::uniform_real_distribution<> large_dist(10, 50);
  std::uniform_real_distribution<> wave_dist(0.1, 1);
  t_real waver = wave_dist(*mersenne);
  t_real wavei = wave_dist(*mersenne);
  t_complex waveK(waver, wavei);
  for(t_int n = 0; n < 1; n++) {
    for(t_int m = -n; m <= n; m++) {
      // "Simple singular expanded in regular"
      t_real small = small_dist(*mersenne);
      t_real large = large_dist(*mersenne);
      t_real large_small_diff = large - small;

      check_coaxial_translation_zero(n, m, waveK, false);

      check_coaxial_translation_zero(n, m, waveK, true);

      check_coaxial_translation_onaxis(large, small, false, true, n, m, waveK);
      // "Simple singular expanded in singular"
      check_coaxial_translation_onaxis(large, large_small_diff, false, false, n, m, waveK);
      // "Simple regular expanded in regular"
      check_coaxial_translation_onaxis(large, large_small_diff, true, true, n, m, waveK);
      // "Simple regular expanded in regular"
      check_coaxial_translation_onaxis(large, small, true, true, n, m, waveK);
      // Zero translation
      check_coaxial_translation_onaxis(large, large, true, true, n, m, waveK);

      check_coaxial_translation_onaxis(large, large, false, false, n, m, waveK);
    }
  }
}

Vector<t_real> to_spherical(Vector<t_real> const &x) {
  assert(x.size() == 3);
  auto const r = x.stableNorm();
  auto const theta = std::atan2(x[1], x[0]);
  auto const phi = std::atan2(std::sqrt(x[0] * x[0] + x[1] * x[1]), x[2]);
  Vector<t_real> result(3);
  result << r, phi, theta > 0 ? theta : theta + 2 * constant::pi;
  return result;
};

t_complex
basis_function(t_complex waveK, bool expansion, Vector<t_real> const &r, t_int n, t_int m) {
  auto const function = expansion ? optimet::bessel<Bessel> : optimet::bessel<Hankel1>;
  auto const spherical = to_spherical(r);
  auto const result = std::get<0>(function(spherical(0) * waveK, n)).back() *
    boost::math::spherical_harmonic(n, m, spherical(1), spherical(2));
  return result;
};

t_complex nonradiating_basis(t_complex waveK, Vector<t_real> const &r, t_int n, t_int m) {
  return basis_function(waveK, true, r, n, m);
}
t_complex radiating_basis(t_complex waveK, Vector<t_real> const &r, t_int n, t_int m) {
  return basis_function(waveK, false, r, n, m);
}

TEST_CASE("Translation of two spheres") {
  // Create a geometry with two spheres of reasonable size w.r.t. wavelength and number of harmonics
  // Check that translated potential inside second sphere is consistent with original field
  auto const N = 5;
  auto const wavelength = 1000.0;
  std::uniform_real_distribution<> distance_distribution(0, wavelength * 2e0);
  auto const radius0 = distance_distribution(*mersenne) + wavelength * 0.1;
  auto const radius1 = distance_distribution(*mersenne) + wavelength * 0.1;
  auto const separation = distance_distribution(*mersenne);

  Eigen::Matrix<t_real, 3, 1> const direction = Eigen::Matrix<t_real, 3, 1>::Random().normalized();
  Eigen::Matrix<t_real, 3, 1> const r_q(0, 0, 0);
  Eigen::Matrix<t_real, 3, 1> const r_p(0, 0, radius0 + radius1 + separation);
  // get a point inside second sphere
  std::uniform_real_distribution<> inner_distribution(0, radius1);
  Eigen::Matrix<t_real, 3, 1> const Mt = direction * inner_distribution(*mersenne);
  Eigen::Matrix<t_real, 3, 1> const M = r_p - r_q + Mt;

  // Random potential to translate
  auto const potential = Vector<t_complex>::Random(N * (N + 2) + 1).real().eval();
  SECTION("Radiating to non-radiating") {
    // compute potential at M
    t_complex pot_M = 0e0, pot_Mt = 0e0;
    CachedCoAxialRecurrence tca((r_p - r_q).stableNorm(), 1.0 / wavelength, false);
    for(t_int n(0), i(0); n <= N; ++n)
      for(t_int m(-n); m <= n; ++m, ++i) {
        pot_M += radiating_basis(1e0 / wavelength, M, n, m) * potential(i);
        for(t_int l(std::abs(m)); l <= N + 24; ++l) {
          pot_Mt += tca(n, m, l) * nonradiating_basis(1e0 / wavelength, Mt, l, m) * potential(i);
        }
      }
    CHECK(pot_Mt.real() == Approx(pot_M.real()));
    CHECK(pot_Mt.imag() == Approx(pot_M.imag()));
  }
  SECTION("Non-radiating to non-radiating") {
    // compute potential at M
    t_complex pot_M = 0e0, pot_Mt = 0e0;
    CachedCoAxialRecurrence tca((r_p - r_q).stableNorm(), 1.0 / wavelength, true);
    for(t_int n(0), i(0); n <= N; ++n)
      for(t_int m(-n); m <= n; ++m, ++i) {
        pot_M += nonradiating_basis(1e0 / wavelength, M, n, m) * potential(i);
        for(t_int l(std::abs(m)); l <= N + 24; ++l) {
          pot_Mt += tca(n, m, l) * nonradiating_basis(1e0 / wavelength, Mt, l, m) * potential(i);
        }
      }
    CHECK(pot_Mt.real() == Approx(pot_M.real()));
    CHECK(pot_Mt.imag() == Approx(pot_M.imag()));
  }
}
