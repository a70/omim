#include "testing/testing.hpp"

#include "partners_api/yandex_api.hpp"

#include "geometry/latlon.hpp"

#include "platform/platform.hpp"

namespace
{
UNIT_TEST(Yandex_GetTaxiInfo)
{
  ms::LatLon const from(55.796918, 37.537859);
  ms::LatLon const to(55.758213, 37.616093);
  string result;

  taxi::yandex::RawApi::GetTaxiInfo(from, to, result);

  TEST(!result.empty(), ());
}

UNIT_TEST(Yandex_GetAvailableProducts)
{
  taxi::yandex::Api api("http://localhost:34568/partners");
  ms::LatLon const from(55.796918, 37.537859);
  ms::LatLon const to(55.758213, 37.616093);

  std::vector<taxi::Product> resultProducts;

  api.GetAvailableProducts(from, to,
                           [&resultProducts](std::vector<taxi::Product> const & products) {
                             resultProducts = products;
                             GetPlatform().RunOnGuiThread([] { testing::StopEventLoop(); });
                           },
                           [](taxi::ErrorCode const code) {
                             TEST(false, (code));
                             GetPlatform().RunOnGuiThread([] { testing::StopEventLoop(); });
                           });

  testing::RunEventLoop();

  TEST(!resultProducts.empty(), ());

  taxi::ErrorCode errorCode = taxi::ErrorCode::RemoteError;
  ms::LatLon const farPos(56.838197, 35.908507);
  api.GetAvailableProducts(from, farPos,
                           [](std::vector<taxi::Product> const & products) {
                             TEST(false, ());
                             GetPlatform().RunOnGuiThread([] { testing::StopEventLoop(); });
                           },
                           [&errorCode](taxi::ErrorCode const code) {
                             errorCode = code;
                             GetPlatform().RunOnGuiThread([] { testing::StopEventLoop(); });
                           });

  TEST_EQUAL(errorCode, taxi::ErrorCode::NoProducts, ());

  testing::RunEventLoop();
}
}  // namespace
