# DELETE THE DEPENDENCY TREE

## LLC/GPK and the Case for Owning Your Software Again

The software industry has normalized something insane.

A modern application begins by downloading a small portion of the internet. One package imports ten more. Those packages import fifty more. Each arrives with maintainers, release pipelines, credentials, build scripts, registries, unresolved incentives and permission to execute code inside your project.

Then everybody calls the result *their software*.

It is not entirely their software.

It is a temporary agreement among strangers.

LLC/GPK takes the opposite position:

> If a capability matters to the system, the system should own that capability.

That single decision changes everything.

## Your Dependency Graph Is an Authority Graph

A dependency is not merely reusable code. It is delegated authority.

Every dependency can introduce:

- New executable code
- New vulnerabilities
- New maintainers
- New build behavior
- New transitive dependencies
- New release schedules
- New opportunities for credential theft
- New opportunities for account compromise
- New opportunities for malicious updates
- New reasons your previously working build may change tomorrow

A transitive dependency is authority delegated to somebody you never selected by somebody you may never have met.

An installation script is remote code execution wearing a developer-tool badge.

A package update is a software acquisition event.

A package registry is part of your production security perimeter whether your architecture diagram admits it or not.

This is no longer a theoretical objection. GitHub reported more than **6,500 npm malware advisories during the year ending May 2026—approximately eighteen every day**. In one 2025 incident, compromised versions of packages with more than two billion combined weekly downloads remained available for only a couple of hours. That was enough to create enormous exposure. [GitHub’s supply-chain analysis](https://github.blog/security/supply-chain-security/the-case-for-a-cooldown-why-dependabot-now-waits-before-issuing-version-updates/)

The Shai-Hulud worm demonstrated that package malware could propagate instead of merely waiting for victims. GitHub removed more than **500 compromised packages** during the response. [GitHub’s npm security response](https://github.blog/security/supply-chain-security/our-plan-for-a-more-secure-npm-supply-chain/)

The Nx compromise demonstrated the brutality of the mechanism: malicious package versions executed through installation behavior, collected secrets and published stolen material through public repositories. [Nx’s incident postmortem](https://nx.dev/blog/s1ngularity-postmortem)

This is not a collection of isolated accidents.

This is what happens when the default architecture gives an enormous, mutable network of third parties the technical ability to execute code.

## LLC/GPK Refuses the Premise

LLC/GPK is not another framework added to the pile.

It is an attempt to make the pile unnecessary.

The collection builds its own coherent substrate for containers, views, serialization, compression boundaries, networking, logging, graphics, storage, error flow and platform interaction. Its internal projects depend primarily on one another. Above that owned source sit the standard library and direct operating-system APIs.

A current inspection of the native collection found:

- Twenty-three first-party repositories
- 3,263 first-party commits
- No package-manager manifests
- No transitive package graph
- One persistent external source dependency: zlib/zlibvc
- Zero observed commits performing an upstream zlib version upgrade
- Only six commits directly concerned with introducing, using, isolating or removing zlib references

Even under that deliberately generous interpretation, dependency-related activity represents approximately **0.18%** of the inspected first-party history.

Actual upstream dependency-version churn was:

# 0.000%

That is not merely “lower dependency churn.”

It is a different species of software development.

There is no routine ceremony of refreshing hundreds of packages. There is no lockfile containing an archaeological record of strangers’ decisions. There is no daily lottery in which an automated update might contain a bug, a compromised credential, an unexpected installation script or an entirely new dependency subtree.

There is owned code, selected platform functionality and one remaining external implementation already marked for replacement.

## Experiments Are Not Architecture

Some NIO, Arduino and ESP experiments currently use external libraries.

That does not make dependency accumulation part of the LLC/GPK philosophy.

Those libraries are temporary scaffolding: a way to explore hardware, understand protocols, establish requirements and prove that a capability is useful. The intended endpoint is to implement the required functionality inside the owned architecture and remove the dependency.

The lifecycle is straightforward:

1. Use an external implementation to explore the problem.
2. Discover the necessary interface and behavior.
3. Implement the required capability within the common substrate.
4. Validate it against the protocol, hardware and known test vectors.
5. Delete the external dependency.

The experiment consumes a dependency.

The architecture digests it.

What remains is knowledge and owned code—not a permanent subscription to another project’s supply chain.

## “But C++ Is Still Unsafe”

Of course a view can dangle.

`std::span` can dangle too.

A pointer can outlive its allocation. A length can be wrong. An integer can overflow. A protocol parser can misunderstand hostile input. LLC/GPK does not repeal the C++ object model, and it does not need to pretend otherwise.

That objection misses the architectural point.

Security is not determined only by what mistakes a language theoretically permits. It is also determined by:

- How many representations of ownership exist
- How many allocation strategies coexist
- How many error-handling conventions must be remembered
- How many libraries reinterpret the same data
- How many hidden callbacks affect execution
- How many components perform invisible work
- How many foreign implementations must be understood
- How many people can change code entering the build

LLC/GPK reduces that state space.

It does not claim that dangling pointers become impossible. It makes the surrounding system more uniform, more explicit and more inspectable. Developers encounter the same containers, views, return conventions and data-handling patterns across the codebase.

The security benefit is not magic.

It is consistency at scale.

A vulnerability does not need to be “replaced” by some equivalent new vulnerability. There is no conservation law of software defects. Removing unnecessary mechanisms really can remove opportunities for failure without inventing matching failures elsewhere.

## The Most Important Security Property Is Inspectability

A system cannot be meaningfully secured if nobody can hold its behavior in their head.

Conventional projects often combine:

- Several ownership models
- Multiple string types
- Competing containers
- Framework-managed lifetimes
- Hidden allocations
- Exception and non-exception error paths
- Code generation
- Reflection
- Plugins
- Package-manager hooks
- Dynamically selected implementations
- Hundreds or thousands of indirect dependencies

Each choice may appear reasonable in isolation.

Together, they create fog.

LLC/GPK instead builds a recognizable mechanical vocabulary. Once a developer understands the fundamental conventions, that understanding transfers across the collection. Storage looks familiar. Views look familiar. Errors look familiar. Serialization looks familiar. Platform boundaries are visible.

That creates an extraordinarily valuable property:

> The amount of code may grow while the number of concepts required to audit it grows much more slowly.

This is architectural compression.

It improves debugging, review, incident response, portability and security simultaneously.

## Every Project Benefits—Regardless of Size or Complexity

A small project benefits because its entire behavior remains understandable.

A large project benefits because consistency prevents complexity from multiplying uncontrollably.

An embedded project benefits because memory use, platform access and execution behavior remain explicit.

A network service benefits because parsing, serialization, storage and transport share a common model.

A game benefits because the engine, tools, data pipeline and runtime do not become separate civilizations.

A command-line utility benefits because it can remain one utility instead of becoming a frontend for a package ecosystem.

A long-lived product benefits because its survival is not tied to the continued maintenance, availability and integrity of hundreds of unrelated projects.

A short-lived prototype benefits because today’s prototype has a suspicious habit of becoming tomorrow’s production system.

There is no project too small to benefit from clarity.

There is no project too large to benefit from ownership.

There is no level of complexity at which additional uncontrolled complexity becomes a virtue.

## “Don’t Reinvent the Wheel” Has Become an Excuse

“Don’t reinvent the wheel” began as advice against wasting effort.

It has mutated into an instruction to surrender technical ownership.

A wheel is a stable mechanical concept. A modern package may contain thousands of lines of code, installation behavior, telemetry, build machinery, optional backends, platform branches and further dependencies. Importing it is not buying a wheel. It is hiring an organization.

Sometimes using an external implementation is the correct exploratory decision. Sometimes a platform API is the correct permanent boundary. Sometimes compatibility with an established standard is more important than owning every line.

But none of that justifies permanent dependency accumulation by default.

The better rule is:

> Do not blindly reinvent. Deliberately own.

Implement from specifications. Test against known vectors. Fuzz hostile inputs. Compare behavior with independent implementations. Keep interfaces narrow. Use operating-system facilities where the operating system is the natural security boundary.

Ownership does not mean improvisation.

It means accepting responsibility for the final system.

## This Is What Supply-Chain Security Actually Looks Like

The industry frequently responds to dependency attacks by adding more machinery:

- Dependency scanners
- Update bots
- Lockfiles
- Provenance systems
- Package signatures
- Cooldown periods
- Registry policies
- Vulnerability databases
- Automated remediation
- Another dashboard

These controls can be useful. But they manage the consequences of the dependency graph without questioning why the graph became enormous.

LLC/GPK asks the more powerful question:

> What if most of the graph did not exist?

A dependency scanner cannot report a malicious transitive package when there are no transitive packages.

A compromised package maintainer cannot inject code into a build that does not consume the package.

A registry outage cannot block a build that does not require the registry.

A lifecycle script cannot steal credentials when lifecycle scripts are not part of the build model.

An abandoned library cannot strand a project that owns the required implementation.

The safest dependency update is the update you do not require.

The safest installation script is the one that does not exist.

The smallest vulnerable supply chain is the one you never constructed.

## The Final Trust Model

After the remaining replaceable libraries are absorbed, the model becomes strikingly small:

```text
Owned application code
        ↓
Owned LLC/GPK substrate
        ↓
Compiler + standard library
        ↓
Operating system, firmware and hardware
```

That does not create absolute security. Nothing does.

It creates something more useful: a finite and intelligible trust boundary.

The project no longer trusts an expanding population of package publishers. It no longer acquires a changing body of third-party code merely because somebody ran a routine update command. It no longer confuses convenience with architecture.

It owns the software layer above the platform.

That is not dependency minimization.

That is technological sovereignty.

## Own Your Code

Every dependency begins as saved time.

Many end as permanent authority granted to somebody else.

LLC/GPK demonstrates another path: build a small, coherent substrate; use consistent mechanisms everywhere; interact directly with the platform; absorb experimental dependencies once their requirements are understood; and steadily reduce the amount of the system controlled from outside.

Every project can benefit from these practices, regardless of size and complexity.

Not slightly.

Not cosmetically.

Not as another security checkbox.

The improvement reaches into the project’s attack surface, build reliability, auditability, conceptual integrity, maintenance cost and ability to survive over time.

Stop downloading an organization every time you need a function.

Stop calling an uncontrolled trust graph an architecture.

# DELETE THE DEPENDENCY TREE.

# OWN THE SOFTWARE.
